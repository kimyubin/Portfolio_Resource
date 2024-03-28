// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelDefaultPawn.h"

#include "CitadelInputConfig.h"
#include "CitadelPawnMovementComponent.h"
#include "CitadelPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACitadelDefaultPawn::ACitadelDefaultPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//카메라가 회전(컨트롤러가 회전)할 때 캐릭터가 같이 회전하는 거 방지. 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CitadelMovementComponent = CreateDefaultSubobject<UCitadelPawnMovementComponent>(TEXT("CitadelMovementComponent"));
	CitadelMovementComponent->UpdatedComponent = RootComponent;
	CitadelMovementComponent->MaxSpeed = 1800.f;
	
	// 카메라 암 구성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // 캐릭터가 회전할 때 카메라가 따라 회전하는 거 방지
	CameraBoom->bDoCollisionTest = false;			// 충돌할 때 카메라 암이 줄어드는거 방지
	CameraBoom->bEnableCameraLag = true;			// 카메라 이동시 천천히 따라감
	CameraBoom->bEnableCameraRotationLag = true;	// 카메라 각도 조절할 때 부드럽게 변경
	CameraBoom->CameraLagSpeed = 10.f;
	CameraBoom->CameraRotationLagSpeed = 10.f;
	CameraBoom->TargetOffset = FVector(0.f, 0.f, 100.f);	//타겟의 3차원 중심으로 위치 변경

	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
	PlayerCameraComponent->SetFieldOfView(65.f);	//90.f);
	PlayerCameraComponent->bUsePawnControlRotation = false; // 카메라 암이 회전할 때 회전 방지

	AngleAdjustmentSteps = 14;
	StartAngleStep = 9;
	
	MaxAngle = -80.;
	MinAngle = -24.;
	MaxLength = 3600.f;
	MinLength = 1080.f;
	
	InitSettings();
}

void ACitadelDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetFirstPlayerController()->Possess(this);
	InitSettings();

}
void ACitadelDefaultPawn::InitSettings()
{
	CurrentAngleStep = StartAngleStep;
	ClampAngleStep();
	
	UnitAngle = (MaxAngle - MinAngle) / AngleAdjustmentSteps;
	UnitLength = (MaxLength - MinLength) / AngleAdjustmentSteps;
	SetCameraBoomRelativePitch(GetDesiredAngle());
	CameraBoom->TargetArmLength = GetDesiredArmLength(); //2700.f
}

void ACitadelDefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCameraZoom(DeltaTime);
}

void ACitadelDefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const auto PCInputConfig = Cast<ACitadelPlayerController>(GetController())->InputActionsConfig;
	const auto EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// 빙의할 때 사용할 입력 바인드
	EnhancedInputComp->BindAction(PCInputConfig->Citadel_IA_Move, ETriggerEvent::Triggered, this, &ACitadelDefaultPawn::MoveCamera);
	EnhancedInputComp->BindAction(PCInputConfig->Citadel_IA_WheelAxis, ETriggerEvent::Triggered, this, &ACitadelDefaultPawn::ChangeCameraAngle);
}
UPawnMovementComponent* ACitadelDefaultPawn::GetMovementComponent() const
{
	return CitadelMovementComponent;
}

void ACitadelDefaultPawn::MoveCamera(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>(); 
	
	if( (InputVector.X != 0.f) && ( Controller != NULL ))
	{
		const FRotationMatrix R(GetActorRotation());
		const FVector WorldSpaceAccel = R.GetScaledAxis( EAxis::X );

		AddMovementInput(WorldSpaceAccel, InputVector.X);
	}
	
	if( (InputVector.Y != 0.f) && ( Controller != NULL ))
	{
		const FRotationMatrix R(GetActorRotation());
		const FVector WorldSpaceAccel = R.GetScaledAxis( EAxis::Y );

		AddMovementInput(WorldSpaceAccel, InputVector.Y);
	}
}

void ACitadelDefaultPawn::ChangeCameraAngle(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();

	if ((Controller != nullptr) && (Axis != 0.0f))
	{
		if (Axis < 0)
			CurrentAngleStep++;
		else if (Axis > 0)
			CurrentAngleStep--;
		ClampAngleStep();
		
		const double DesiredAngle = GetDesiredAngle();
		if (!FMath::IsNearlyEqual(DesiredAngle, CameraBoom->GetRelativeRotation().Pitch))
		{
			SetCameraBoomRelativePitch(DesiredAngle);
			bCameraZoom = true;
		}
	}
}

void ACitadelDefaultPawn::SetCameraBoomRelativePitch(double InPitch)
{
	FRotator CurrentSpringArmAngle = CameraBoom->GetRelativeRotation();
	CurrentSpringArmAngle.Pitch = InPitch;
	CameraBoom->SetRelativeRotation(CurrentSpringArmAngle);
}

void ACitadelDefaultPawn::UpdateCameraZoom(const float DeltaTime)
{
	if (bCameraZoom)
	{
		//편의를 위한 스프링 암 길이 레퍼런스
		float& SpringArmLengthRef = CameraBoom->TargetArmLength;		
		const float DesiredLength = GetDesiredArmLength();
		
		// 카메라 서브 스테핑 사용시, 프레임 보정		
		if (CameraBoom->bUseCameraLagSubstepping && DeltaTime > CameraBoom->CameraLagMaxTimeStep)
		{
			//목표 길이와 이전 길이의 간격에
			//델타타임의 역수 = 프레임 수를 곱함. 1초에 움직이는 스텝 수
			const float ArmMovementStep = (DesiredLength - SpringArmLengthRef) * (1.f / DeltaTime);

			float LerpTarget = SpringArmLengthRef;

			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraBoom->CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmMovementStep * LerpAmount;
				RemainingTime -= LerpAmount;

				SpringArmLengthRef = FMath::FInterpTo(SpringArmLengthRef, LerpTarget, LerpAmount, CameraBoom->CameraLagSpeed);
			}
		}
		else
		{
			// 일반 선형보간 이동.
			SpringArmLengthRef = FMath::FInterpTo(SpringArmLengthRef, DesiredLength, DeltaTime, CameraBoom->CameraLagSpeed);
		}
		
		if (FMath::IsNearlyEqual(DesiredLength, SpringArmLengthRef))
			bCameraZoom = false;
	}
}

void ACitadelDefaultPawn::GoToActor(const FVector& TargetPosition)
{
	SetDefaultPawnLocation(TargetPosition);
}

void ACitadelDefaultPawn::SetDefaultPawnLocation(const FVector& TargetPosition)
{
	FVector ConvertLocation = TargetPosition;
	ConvertLocation.Z = GetActorLocation().Z;
	SetActorLocation(ConvertLocation);
}

double ACitadelDefaultPawn::GetDesiredAngle() const
{
	return MinAngle + UnitAngle * CurrentAngleStep;
}

float ACitadelDefaultPawn::GetDesiredArmLength() const
{
	return MinLength + UnitLength * CurrentAngleStep;
}

void ACitadelDefaultPawn::ClampAngleStep()
{
	CurrentAngleStep = FMath::Clamp(CurrentAngleStep, 0, AngleAdjustmentSteps);
}