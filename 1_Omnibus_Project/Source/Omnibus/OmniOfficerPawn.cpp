// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniOfficerPawn.h"

#include "Omnibus.h"
#include "OmnibusGameInstance.h"
#include "OmnibusInputConfig.h"
#include "OmnibusPlayerController.h"
#include "OmnibusRoadManager.h"
#include "OmnibusUtilities.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

AOmniOfficerPawn::AOmniOfficerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//카메라가 회전(컨트롤러가 회전)할 때 캐릭터가 같이 회전하는 거 방지. 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MovementComponent = CreateDefaultSubobject<USpectatorPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->MaxSpeed         = 1800.f;
	MovementComponent->Acceleration     = 4000.0f;
	MovementComponent->Deceleration     = 12000.0f;

	CameraMoveSpeed = MovementComponent->MaxSpeed;
	Acceleration    = MovementComponent->Acceleration;
	Deceleration    = MovementComponent->Deceleration;

	// 카메라 암 구성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0)); // Y축 음수 방향이 화면 위로 오게 변경.
	CameraBoom->TargetArmLength  = 1500.0f;
	CameraBoom->bDoCollisionTest = false;                       // 충돌할 때 카메라 암이 줄어드는거 방지
	CameraBoom->bEnableCameraLag = false;                       // 카메라 이동랙 없음.
	
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->SetRelativeRotation(FRotator(0.0, 0.0, -90.0));
	PlayerCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	PlayerCamera->SetOrthoNearClipPlane(-768.0f);
	PlayerCamera->SetAutoCalculateOrthoPlanes(true);
	PlayerCamera->OrthoWidth = 1920.0f;
	PlayerCamera->bUsePawnControlRotation = false; // 카메라 암이 회전할 때 회전 방지

	ZoomSpeed     = 10.f;
	MaxZoomStep   = 17;
	StartZoomStep = 14;

	MaxOrthoWidth = 48000.f;
	MinOrthoWidth = 4500.f;
	InitSettings();

	bCameraZooming                = false;
	bUsingMouseWheelCameraZoom    = false;
	PrevScrollUnderCursorLocation = FVector2D::ZeroVector;

	bDragActive                   = false;
	DragStartMouseLocation        = FVector2D::ZeroVector;

	PassengerVisibilityMaxStep    = MaxZoomStep - 1;
	PassengerVisibilityOrthoWidth = MaxOrthoWidth - MinOrthoWidth;
}

void AOmniOfficerPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InitSettings();
	
}

void AOmniOfficerPawn::InitSettings()
{
	CurrentZoomStep = FMath::Clamp(StartZoomStep, 0, MaxZoomStep);

	// 단계별 증가율의 기하평균. (전체 증가율의 n제곱근(줌단계))
	// MinOrthoWidth * (StepIntervalRate^(MaxZoomStep)) = MaxOrthoWidth
	// 확대 축소 비율을 동일하게 하기 위함.
	const float StepIntervalRate = pow(MaxOrthoWidth / MinOrthoWidth, 1.0 / static_cast<float>(MaxZoomStep));

	OrthoWidths.clear();
	OrthoWidths.reserve(MaxZoomStep + 1);
	OrthoWidths.push_back(MinOrthoWidth);

	for (int step = 0; step < MaxZoomStep; ++step)
	{
		OrthoWidths.push_back(OrthoWidths.back() * StepIntervalRate);
	}
	
	PlayerCamera->OrthoWidth = GetDesiredOrthoWidth();

	// Passenger 렌더링 기준 단계 계산
	OB_LIGHT_CHECK(PassengerVisibilityMaxStep < OrthoWidths.size() - 1, "PassengerVisibilityMaxStep is too big. Step must be less than %llu.", (OrthoWidths.size() - 1))

	PassengerVisibilityMaxStep    = FMath::Clamp(PassengerVisibilityMaxStep, 0, OrthoWidths.size() - 2);
	PassengerVisibilityOrthoWidth = (OrthoWidths[PassengerVisibilityMaxStep + 1] + OrthoWidths[PassengerVisibilityMaxStep]) / 2;
}

void AOmniOfficerPawn::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetFirstPlayerController()->Possess(this);
	SetActorRotation(FRotator(0.0, -90.0, 0.0));
	
}

void AOmniOfficerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraZoom(GetWorld()->GetTime().GetDeltaRealTimeSeconds()/*DeltaTime*/);
}

void AOmniOfficerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const UOmnibusInputConfig* PCInputConfig   = Cast<AOmnibusPlayerController>(GetController())->InputActionsConfig;
	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComp->BindAction(PCInputConfig->Omni_IA_Point_Zoom, ETriggerEvent::Triggered, this, &AOmniOfficerPawn::ZoomCameraAtCursor);
	EnhancedInputComp->BindAction(PCInputConfig->Omni_IA_Center_Zoom, ETriggerEvent::Triggered, this, &AOmniOfficerPawn::ZoomCameraCenter);
	EnhancedInputComp->BindAction(PCInputConfig->Omni_IA_MoveCamera, ETriggerEvent::Triggered, this, &AOmniOfficerPawn::MoveCameraInput);
}

UPawnMovementComponent* AOmniOfficerPawn::GetMovementComponent() const
{
	return MovementComponent;
}

float AOmniOfficerPawn::GetSpringArmLength() const
{
	return CameraBoom->TargetArmLength;
}

void AOmniOfficerPawn::MoveCameraInput(const FInputActionValue& InputValue)
{
	const FVector2D InputVector = InputValue.Get<FVector2D>();
	const float WidthRatio      = PlayerCamera->OrthoWidth / OrthoWidths.front();

	MovementComponent->MaxSpeed     = CameraMoveSpeed * WidthRatio;
	MovementComponent->Acceleration = Acceleration * WidthRatio;
	MovementComponent->Deceleration = Deceleration * WidthRatio;

	if (OB_IS_VALID(Controller))
	{
		AddMovementInput(GetActorRightVector(), InputVector.Y);
		AddMovementInput(GetActorForwardVector(), InputVector.X);
	}
}

void AOmniOfficerPawn::SetDragActive(const bool bInDragActive, const FVector2D& InMousePosition)
{
	bDragActive = bInDragActive;
	if (bDragActive)
		DragStartMouseLocation = InMousePosition;
}

void AOmniOfficerPawn::DragMap(const FVector2D& InMousePosition)
{
	if (bDragActive)
	{
		FVector2D ViewportSize;
		if (OB_IS_VALID(GetWorld()) && OB_IS_VALID(GetWorld()->GetGameViewport()))
			GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

		// 화면이 커서를 따라가도록, 이동 속도 조절.
		const float     MoveSpeed      = PlayerCamera->OrthoWidth / ViewportSize.X;
		const FVector2D MouseDelta     = InMousePosition - DragStartMouseLocation;
		const FVector   MovementVector = ((GetActorRightVector() * MouseDelta.X * -1) + (GetActorForwardVector() * MouseDelta.Y)) * MoveSpeed;

		SetDefaultPawnLocation(GetActorLocation() + MovementVector);

		DragStartMouseLocation = InMousePosition;
	}
}

void AOmniOfficerPawn::ZoomCameraAtCursor(const FInputActionValue& InputValue)
{
	const float Axis = InputValue.Get<float>();
	if (OB_IS_VALID(Controller))
	{
		//상하 제한에 걸리면 줌 실행안함.
		if (AddZoomStepByAxis(Axis))
		{
			bCameraZooming             = true;
			bUsingMouseWheelCameraZoom = true;

			//마우스 휠업다운인 경우 커서 중심으로 줌하기 위해 커서 위치 저장.
			FHitResult Result;
			GetController<AOmnibusPlayerController>()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Result);
			PrevScrollUnderCursorLocation = OmniMath::MakeFVector2D(Result.Location);
		}
	}
}

void AOmniOfficerPawn::ZoomCameraCenter(const FInputActionValue& InputValue)
{
	const float Axis = InputValue.Get<float>();
	if (OB_IS_VALID(Controller))
	{
		//상하 제한에 걸리면 줌 실행안함.
		if (AddZoomStepByAxis(Axis))
		{
			bCameraZooming = true;
			bUsingMouseWheelCameraZoom = false;
		}
	}
}

void AOmniOfficerPawn::UpdateCameraZoom(const float DeltaTime)
{
	if (bCameraZooming)
	{
		const float DesiredOrthoWidth = GetDesiredOrthoWidth();
		const float PrevOrthoWidth    = PlayerCamera->OrthoWidth;
		PlayerCamera->OrthoWidth      = FMath::FInterpTo(PlayerCamera->OrthoWidth, DesiredOrthoWidth, DeltaTime, ZoomSpeed);

		// 마우스 휠업다운인 경우 마우스 중심으로 확대 축소하기 위해 폰 위치 변경.
		if (bUsingMouseWheelCameraZoom)
		{
			//마우스 커서가 가리키는 위치 고정
			const FVector2D ActorLocation2D = OmniMath::MakeFVector2D(GetActorLocation());
			const float ScreenRatio         = PlayerCamera->OrthoWidth / PrevOrthoWidth;
			const FVector2D LerpActorLoc2D  = FMath::Lerp(PrevScrollUnderCursorLocation, ActorLocation2D, ScreenRatio);

			SetDefaultPawnLocation(OmniMath::MakeFVector(LerpActorLoc2D));
		}

		if (FMath::IsNearlyEqual(PlayerCamera->OrthoWidth, DesiredOrthoWidth, 1.e-3f))
		{
			PlayerCamera->OrthoWidth   = DesiredOrthoWidth;
			bCameraZooming             = false;
			bUsingMouseWheelCameraZoom = false;
		}


		// 기준 너비보다 OrthoWidth가 커지면, passenger 렌더링 비활성화.
		if (PrevOrthoWidth <= PassengerVisibilityOrthoWidth && PassengerVisibilityOrthoWidth < PlayerCamera->OrthoWidth)
		{
			AOmnibusRoadManager* RoadManager = GetGameInstance<UOmnibusGameInstance>()->GetOmnibusRoadManager();
			if (IsValid(RoadManager))
				RoadManager->SetPassengerRender(false);
		}
		else if (PrevOrthoWidth > PassengerVisibilityOrthoWidth && PassengerVisibilityOrthoWidth >= PlayerCamera->OrthoWidth)
		{
			AOmnibusRoadManager* RoadManager = GetGameInstance<UOmnibusGameInstance>()->GetOmnibusRoadManager();
			if (IsValid(RoadManager))
				RoadManager->SetPassengerRender(true);
		}
	}
}

float AOmniOfficerPawn::GetDesiredOrthoWidth() const
{
	return OrthoWidths[FMath::Clamp(CurrentZoomStep, 0, OrthoWidths.size() - 1)];
}

bool AOmniOfficerPawn::AddZoomStep(const int InZoomStepDelta)
{
	if (InZoomStepDelta < 0)
	{
		//줌 아웃
		if (CurrentZoomStep < MaxZoomStep)
		{
			CurrentZoomStep++;
			return true;
		}
	}
	else if (InZoomStepDelta > 0)
	{
		//줌 인
		if (CurrentZoomStep > 0)
		{
			CurrentZoomStep--;
			return true;
		}
	}

	return false;
}

bool AOmniOfficerPawn::AddZoomStepByAxis(const float InZoomStepAxis)
{
	if (InZoomStepAxis < 0)
		return AddZoomStep(-1);

	if (InZoomStepAxis > 0)
		return AddZoomStep(1);

	return false;
}

void AOmniOfficerPawn::GoToActor(const FVector& TargetPosition)
{
	SetDefaultPawnLocation(TargetPosition);
}

void AOmniOfficerPawn::SetDefaultPawnLocation(const FVector& TargetPosition)
{
	FVector ConvertLocation = TargetPosition;
	ConvertLocation.Z       = GetActorLocation().Z;
	SetActorLocation(ConvertLocation);
}
