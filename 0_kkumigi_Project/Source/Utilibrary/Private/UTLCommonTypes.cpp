// Fill out your copyright notice in the Description page of Project Settings.


#include "UTLCommonTypes.h"

#include "GameFramework/SpringArmComponent.h"

#include "UtlLog.h"
#include "UTLStatics.h"

USpringArmControlComponent::USpringArmControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 최초 스폰 시 카메라 위치 보정을 위해 상대좌표 사용.
	SetUsingAbsoluteLocation(false);

	SpringArmWeakPtr = nullptr;

	bWheelUpToCamUp = true;

	AngleAdjustmentSteps = 0;
	StartAngleStep       = 0;
	MaxAngle             = 0;
	MinAngle             = 0;
	MaxLength            = 0;
	MinLength            = 0;

	CurrentAngleStep     = 0;
	UnitAngle            = 0;
	UnitLength           = 0;
	bCameraZoom          = false;
}

void USpringArmControlComponent::SetDefaultConfig(USpringArmComponent* InSpringArmComponent
                                                , const FVector& InCameraDeadZoneMin
                                                , const FVector& InCameraDeadZoneMax
                                                , const uint8 InAngleAdjustmentSteps
                                                , const uint8 InStartAngleStep
                                                , const float InMaxAngle
                                                , const float InMinAngle
                                                , const float InMaxLength
                                                , const float InMinLength
                                                , const bool InbWheelUpToCamUp)
{
	SpringArmWeakPtr = InSpringArmComponent;

	CameraDeadZoneMin = InCameraDeadZoneMin;
	CameraDeadZoneMax = InCameraDeadZoneMax;

	bWheelUpToCamUp = InbWheelUpToCamUp;

	AngleAdjustmentSteps = InAngleAdjustmentSteps;
	StartAngleStep       = InStartAngleStep;
	MaxAngle             = InMaxAngle;
	MinAngle             = InMinAngle;
	MaxLength            = InMaxLength;
	MinLength            = InMinLength;

	InitAngleSettings();
}

void USpringArmControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsUsingAbsoluteLocation() == false)
	{
		if ((CameraDeadZoneMin == FVector::ZeroVector && CameraDeadZoneMax == FVector::ZeroVector) == false)
		{
			// 현재 카메라 데드존이 활성화되어 있지만, 상대위치를 사용 중입니다.
			// 카메라 데드존은 절대 위치를 사용해야합니다.
			UT_LOG("'camera dead zone' is activated, but relative position is being used. 'camera dead zone' must use absolute location.")
		}
	}
	else
	{
		if (const USceneComponent* Parent = GetAttachParent())
		{
			SetWorldLocation(Parent->GetComponentLocation());
		}
	}

	// 커스텀 상대좌표 사용을 위해 절대 좌표 사용.
	SetUsingAbsoluteLocation(bUsedCameraDeadZone);

	InitAngleSettings();
}

void USpringArmControlComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitAngleSettings();
}

void USpringArmControlComponent::InitAngleSettings()
{
	USpringArmComponent* const SpringArm = SpringArmWeakPtr.Get();
	if (SpringArm == nullptr)
	{
		UT_LOG("SpringArm is not exists!")
		return;
	}

	CurrentAngleStep = StartAngleStep;
	ClampAngleStep();
	
	UnitAngle = (MaxAngle - MinAngle) / AngleAdjustmentSteps;
	UnitLength = (MaxLength - MinLength) / AngleAdjustmentSteps;
	SetSpringArmRelativePitch(GetDesiredAngle());
	SpringArm->TargetArmLength = GetDesiredArmLength(); //2700.f
}

void USpringArmControlComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateLocationInDeadZone(DeltaTime);

	TickArmLength(DeltaTime);
}

void USpringArmControlComponent::UpdateLocationInDeadZone(const float DeltaTime)
{
	// 절대 위치를 사용할 때만 동작합니다.
	if (bUsedCameraDeadZone == false)
	{
		return;
	}

	const FVector ParentWorldLocation = GetAttachParent()->GetComponentLocation();
	const FVector CurrentRelLocation  = GetComponentLocation() - ParentWorldLocation;
	const FVector NewRelLocation      = UtlMath::ClampVector3D(CurrentRelLocation, CameraDeadZoneMin, CameraDeadZoneMax);
	if (CurrentRelLocation != NewRelLocation)
	{
		SetWorldLocation(ParentWorldLocation + NewRelLocation);
	}
}

void USpringArmControlComponent::TickArmLength(const float DeltaTime)
{
	if (bCameraZoom == false)
	{
		return;
	}

	USpringArmComponent* const SpringArm = SpringArmWeakPtr.Get();
	if (SpringArm == nullptr)
	{
		return;
	}

	//편의를 위한 스프링 암 길이 레퍼런스
	float& TargetArmLengthRef = SpringArm->TargetArmLength;
	const float DesiredLength = GetDesiredArmLength();

	// 카메라 서브 스테핑 사용시, 프레임 보정		
	if (SpringArm->bUseCameraLagSubstepping && DeltaTime > SpringArm->CameraLagMaxTimeStep)
	{
		// 목표 길이와 이전 길이의 간격에
		// 델타타임의 역수 = 프레임 수를 곱함. 1초에 움직이는 스텝 수
		const float ArmMovementStep = (DesiredLength - TargetArmLengthRef) * (1.f / DeltaTime);

		float LerpTarget = TargetArmLengthRef;

		float RemainingTime = DeltaTime;
		while (RemainingTime > UE_KINDA_SMALL_NUMBER)
		{
			const float LerpAmount = FMath::Min(SpringArm->CameraLagMaxTimeStep, RemainingTime);
			LerpTarget += ArmMovementStep * LerpAmount;
			RemainingTime -= LerpAmount;

			TargetArmLengthRef = FMath::FInterpTo(TargetArmLengthRef, LerpTarget, LerpAmount, SpringArm->CameraLagSpeed);
		}
	}
	else
	{
		// 일반 선형보간 이동.
		TargetArmLengthRef = FMath::FInterpTo(TargetArmLengthRef, DesiredLength, DeltaTime, SpringArm->CameraLagSpeed);
	}

	if (FMath::IsNearlyEqual(DesiredLength, TargetArmLengthRef))
	{
		TargetArmLengthRef = DesiredLength;
		bCameraZoom        = false;
	}
}

void USpringArmControlComponent::ChangeCameraAngleInput(float InInputAxis)
{
	if (InInputAxis == 0.0f)
	{
		return;
	}

	const USpringArmComponent* SpringArm = SpringArmWeakPtr.Get();
	if (SpringArm == nullptr)
	{
		return;
	}


	if (bWheelUpToCamUp == false)
	{
		InInputAxis *= -1;
	}

	if (InInputAxis > 0)
	{
		CurrentAngleStep++;
	}
	else if (InInputAxis < 0)
	{
		CurrentAngleStep--;
	}

	ClampAngleStep();

	const double DesiredAngle = GetDesiredAngle();
	if (FMath::IsNearlyEqual(DesiredAngle, SpringArm->GetRelativeRotation().Pitch) == false)
	{
		SetSpringArmRelativePitch(DesiredAngle);
		bCameraZoom = true;
	}
}

bool USpringArmControlComponent::IsValid() const
{
	return (SpringArmWeakPtr.IsValid());
}

void USpringArmControlComponent::SetSpringArmRelativePitch(const double InPitch)
{
	USpringArmComponent* const SpringArm = SpringArmWeakPtr.Get();
	if (SpringArm == nullptr)
	{
		return;
	}

	FRotator CurrentSpringArmAngle = SpringArm->GetRelativeRotation();
	CurrentSpringArmAngle.Pitch    = InPitch;
	SpringArm->SetRelativeRotation(CurrentSpringArmAngle);
}

double USpringArmControlComponent::GetDesiredAngle() const
{
	return MinAngle + UnitAngle * CurrentAngleStep;
}

float USpringArmControlComponent::GetDesiredArmLength() const
{
	return MinLength + UnitLength * CurrentAngleStep;
}

void USpringArmControlComponent::ClampAngleStep()
{
	CurrentAngleStep = FMath::Clamp(CurrentAngleStep, 0, AngleAdjustmentSteps);
}
