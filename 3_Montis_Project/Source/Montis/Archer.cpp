// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Archer.h"

#include "CrossHairWidget.h"
#include "MontisGameInstance.h"
#include "EnhancedInputComponent.h"
#include "LongBowAnimInstance.h"
#include "Montis.h"
#include "MontisGameUIsHandler.h"
#include "MontisInputConfig.h"
#include "MontisPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

AArcher::AArcher()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 컨트롤러가 회전할 때 자체 회전 금지. 조준 중일 때만 캐릭터 무브먼트 통해 부드러운 회전 적용.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;	// 컨트롤러 방향으로 회전. 조준 중일 땐 같이 회전함.
	GetCharacterMovement()->bOrientRotationToMovement = true;		// 가속 방향으로 회전.
	GetCharacterMovement()->RotationRate = FRotator(0.0, 500.0, 0.0);  // 기본값. 회전속도. 초당 회전 변화량. 속도 변경가능.

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	// 카메라 암 구성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);		// 캐릭터가 회전할 때 카메라가 따라 회전하는 거 방지
	CameraBoom->bDoCollisionTest = true;			// 충돌할 때 카메라 암이 줄듦
	CameraBoom->bUsePawnControlRotation = true;		// 컨트롤러 회전값에 따라 회전함.
	CameraBoom->TargetArmLength = 240.f;
	CameraBoom->SocketOffset = FVector(0.0, 42.0, 65.0);	//카메라의 상대위치. 캐릭터가 회전해도 위치가 바뀌지 않음. TargetOffset은 바뀜. 카메라 위치를 카메라 시선 방향의 상대위치로 고정.

	SetSmoothCameraRotation(false);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // 컨트롤러 따라 회전하는 거 방지.

	ZoomTargetFOV = 40.0;
	ZoomSpeed = 10.0;
	ZoomRate = 1.0 / 3.0;
	ZoomTargetSocketOffset = FVector(0.0, 60.0, 55.0);

	DefaultSocketOffset = CameraBoom->SocketOffset;
	DefaultFOV = CameraComponent->FieldOfView;
	DefaultCameraArmLength = CameraBoom->TargetArmLength;
	
	IsAimed = false;
	IsShoot = false;
	IsTestFOV = false;
}

void AArcher::BeginPlay()
{
	Super::BeginPlay();
	SetUICameraArmLengthAndFOV();

	//BP에서 수정된 값 적용.
	DefaultSocketOffset = CameraBoom->SocketOffset;
	DefaultFOV = CameraComponent->FieldOfView;
	DefaultCameraArmLength = CameraBoom->TargetArmLength;
}

void AArcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FOV 테스트용.
	if(	IsTestFOV )
		return;
	//조준 중 화각, 카메라 암 길이 조절.	
	if (IsAimed)
	{
		if (CameraComponent->FieldOfView > ZoomTargetFOV)
		{
			CameraComponent->FieldOfView = FMath::FInterpTo(CameraComponent->FieldOfView, ZoomTargetFOV, DeltaTime, ZoomSpeed);

			//캐릭터 허리만큼 확대하기 위해 1/3지점까지 암 길이를 줄임.
			CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, GetZoomArmLengthByFOV(), DeltaTime, ZoomSpeed);

			//조준할 때 방해되지 않게 캐릭터 옆으로 카메라 이동.
			CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset,ZoomTargetSocketOffset,DeltaTime,ZoomSpeed);

			SetSmoothCameraRotation(true);
			SetUICameraArmLengthAndFOV();
		}
		else
		{
			SetSmoothCameraRotation(false);
		}
	}
	else
	{
		if (CameraComponent->FieldOfView < DefaultFOV)
		{
			CameraComponent->FieldOfView = FMath::FInterpTo(CameraComponent->FieldOfView, DefaultFOV, DeltaTime, ZoomSpeed);
			CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, GetDefaultArmLengthByFOV(), DeltaTime, ZoomSpeed);

			CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, DefaultSocketOffset, DeltaTime, ZoomSpeed);

			SetSmoothCameraRotation(true);
			SetUICameraArmLengthAndFOV();
		}
		else
		{
			SetSmoothCameraRotation(false);
		}
	}
}

void AArcher::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto MontisPC = Cast<AMontisPlayerController>(GetController()))
	{
		const auto PCInputConfig = MontisPC->InputActionsConfig;
		const auto EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
		
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_Look, ETriggerEvent::Triggered, this, &AArcher::Look);		
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_Move, ETriggerEvent::Triggered, this, &AArcher::MoveCharacter);		
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_Aim, ETriggerEvent::Triggered, this, &AArcher::AimBow);
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_Shoot, ETriggerEvent::Triggered, this, &AArcher::ShootBow);

		//Test
		EnhancedInputComp->BindAction(PCInputConfig->Montis_IA_CameraTester, ETriggerEvent::Triggered, this, &AArcher::ChangeDistanceAndFOV);		

		
	}
}

void AArcher::MoveCharacter(const FInputActionInstance& Instance)
{
	// input is a Vector2D
	FVector2D MovementVector = Instance.GetValue().Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AArcher::Look(const FInputActionInstance& Instance)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Instance.GetValue().Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AArcher::AimBow(const FInputActionInstance& Instance)
{
	IsTestFOV = false;
	
	IsAimed = Instance.GetValue().Get<bool>();
	USkeletalMeshComponent* OwnMesh = GetMesh();
	auto UIHandler = GetGameInstance<UMontisGameInstance>()->GetMontisGameUIsHandler();

	// 조준 애니메이션
	if (IsValid(OwnMesh))
	{
		auto LongBowAnim = Cast<ULongBowAnimInstance>(OwnMesh->GetAnimInstance());
		LongBowAnim->SetAimed(IsAimed);
	}

	//조준선 UI 애니메이션
	if (IsValid(UIHandler))
	{
		UIHandler->GetCrossHairWidget()->PlayAimingAnim(IsAimed);
	}

	LockCamera(IsAimed);
	// 오프셋 변경 후에도 이전 조준 대상 방향으로 컨트롤러 회전
	if (auto MontisPC = Cast<AMontisPlayerController>(GetController()))
	{
		//변경 후 카메라 오프셋
		//스프링 암은 x축 뒤쪽으로 늘어나기 때문에, -1 곱해줌
		FVector TargetCameraOffset;
		if(IsAimed)
		{
			TargetCameraOffset = ZoomTargetSocketOffset;
			TargetCameraOffset.X = -1.0 * GetZoomArmLengthByFOV();
		}
		else
		{
			TargetCameraOffset = DefaultSocketOffset;
			TargetCameraOffset.X = -1.0 * GetDefaultArmLengthByFOV();
		}
		MontisPC->RotateCameraBoomToTarget(GetActorLocation(), TargetCameraOffset);
	}
	
}
void AArcher::ShootBow(const FInputActionInstance& Instance)
{
	IsTestFOV = false;

	IsShoot = Instance.GetValue().Get<bool>();
	USkeletalMeshComponent* OwnMesh = GetMesh();

	if(IsValid(OwnMesh))
	{
		auto LongBowAnim = Cast<ULongBowAnimInstance>(OwnMesh->GetAnimInstance());
		LongBowAnim->SetShoot(IsShoot);	
	}
	LockCamera(IsShoot);
}

void AArcher::LockCamera(bool bLock)
{
	if(bLock)
	{		
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;	// 컨트롤러 방향으로 회전.
		GetCharacterMovement()->bOrientRotationToMovement = true;		// 가속 방향으로 회전.
	}		
}

void AArcher::ChangeDistanceAndFOV(const FInputActionInstance& Instance)
{
	IsTestFOV = true;
	FVector2D CameraModify = Instance.GetValue().Get<FVector2D>();
	float NextArmLength = CameraBoom->TargetArmLength;	//d_2 = \frac{\tan(θ_1)\times d_1}{\tan(θ_2)}
	float NextFOV = CameraComponent->FieldOfView;		//θ_2 = \arctan( \frac{\tan(θ_1)\times d_1}{d_2})
	
	if (Controller != nullptr)
	{
		//위, 아래 Y축. 위(+1)일 경우, 카메라가 앞으로 가면서, 스프링암 길이가 짧아짐.
		if (CameraModify.Y != 0)
		{
			NextArmLength += -CameraModify.Y;
			NextFOV = ComputeFOVForSameSubject(NextArmLength);			
		}

		//좌, 우 X축. 우(+1)일 경우, 슬라이드 오른쪽으로 옮김. 시야각 넓어짐.
		if (CameraModify.X != 0)
		{
			NextFOV += CameraModify.X * 0.5;
			NextArmLength = ComputeArmLengthForSameSubject(NextFOV);
		}
		//경계 검사
		if(NextFOV > 1.f && NextFOV < 180.f)
		{
			CameraBoom->TargetArmLength = NextArmLength;
			CameraComponent->FieldOfView = NextFOV;
		}
	}

	SetUICameraArmLengthAndFOV();
}

double AArcher::ComputeFOVForSameSubject(const double& TargetArmLength)
{
	/* 수식
	 * return = (atan(tan(DefaultFOV / 2 * UE_DOUBLE_PI / 180) * DefaultCameraArmLength / ArmLength) / UE_DOUBLE_PI) * 180 * 2;
	 * θ_2 = \arctan( \frac{\tan(θ_1)\times d_1}{d_2})
	 */	

	// 계산을 위해 각도의 절반만 사용함.
	// 고정된 값. 최초 1회만 계산하는 용도	
	static double HalfDegreeToRadian = 0.5 * UE_DOUBLE_PI / 180.0;
	static double DefaultFovDistConstance = tan(DefaultFOV * HalfDegreeToRadian) * DefaultCameraArmLength;
	static double TwiceRadianToDegree = 180.0 * 2.0 / UE_DOUBLE_PI;

	return atan(DefaultFovDistConstance / TargetArmLength) * TwiceRadianToDegree;
}

double AArcher::ComputeArmLengthForSameSubject(const double& TargetFOV)
{
	/* 수식
	 * return = tan(DefaultFOV / 2 * UE_DOUBLE_PI / 180) * DefaultCameraArmLength / tan(TargetFOV / 2 * UE_DOUBLE_PI / 180);
	 * d_2 = \frac{\tan(θ_1)\times d_1}{\tan(θ_2)} 
	 */	

	// 계산을 위해 각도의 절반만 사용함.
	// 고정된 값. 최초 1회만 계산하는 용도
	static double HalfDegreeToRadian = 0.5 * UE_DOUBLE_PI / 180.0;
	static double DefaultFovDistConstance = tan(DefaultFOV * HalfDegreeToRadian) * DefaultCameraArmLength;
	
	return  DefaultFovDistConstance / tan(TargetFOV * HalfDegreeToRadian);
}

double AArcher::GetDefaultArmLengthByFOV()
{
	return ComputeArmLengthForSameSubject(DefaultFOV);
}

double AArcher::GetZoomArmLengthByFOV()
{
	return ComputeArmLengthForSameSubject(ZoomTargetFOV) * ZoomRate;
}

void AArcher::SetUICameraArmLengthAndFOV()
{
	auto UI_Handler = GetGameInstance<UMontisGameInstance>()->GetMontisGameUIsHandler();
	if(IsValid(UI_Handler))
		UI_Handler->GetCrossHairWidget()->SetCameraDistanceAndFOV(CameraBoom->TargetArmLength, CameraComponent->FieldOfView);
}


void AArcher::SetSmoothCameraRotation(bool IsLag)
{
	if(IsLag)
	{
		CameraBoom->bEnableCameraRotationLag = true; //조준 전 후 카메라 이동시 순간이동 방지
		CameraBoom->CameraRotationLagSpeed = 25.0f;
	}
	else
	{
		CameraBoom->bEnableCameraRotationLag = false;
		CameraBoom->CameraRotationLagSpeed = 100.0f; //랙이 true여도 사용될 기본랙 스피드
	}
}
