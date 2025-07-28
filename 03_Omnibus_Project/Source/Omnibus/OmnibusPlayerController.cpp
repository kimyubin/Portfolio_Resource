// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusPlayerController.h"

#include "OmnibusGameInstance.h"
#include "OmnibusInputConfig.h"
#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmniLineBusRoute.h"
#include "OmniOfficerPawn.h"
#include "OmniStationBusStop.h"
#include "OmniTimeManager.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "Kismet/GameplayStatics.h"

#include "UtlLog.h"
#include "UTLStatics.h"


void AOmnibusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputModeGameUI();

	bEnableTouchEvents     = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents     = true;

	StartBusStop = nullptr;
}

void AOmnibusPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AOmnibusPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* const Subsystem   = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	//입력 다시 맵핑
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_LeftButton, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::LeftButton);
	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_RightButton, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::RightButton);
	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_Drag, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::Drag);

	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_Time_Speed, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::SpeedUpDownGameTime);
	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_Time_Stop, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::ToggleTimeStartPause);
	
	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_ToggleRouteVisibility, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::ToggleRouteVisibility);
}

void AOmnibusPlayerController::GetMultiHitResultsUnderCursorOnOrthographic(const ECollisionChannel TraceChannel, TArray<FHitResult>& OutHitResults) const
{
	const AOmniOfficerPawn* Officer = GetPawn<AOmniOfficerPawn>();
	if (UT_IS_VALID(Officer) == false)
		return;

	// 커서 스크린 좌표를 월드 좌표로 변환.
	FVector2D MousePosition;
	Cast<ULocalPlayer>(Player)->ViewportClient->GetMousePosition(MousePosition);

	FVector ScreenWorldLocation;
	FVector ScreenWorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(this, MousePosition, ScreenWorldLocation, ScreenWorldDirection);

	// 커서의 월드 좌표에서 바라보는 클릭한 지점으로 라인트레이스
	// 직교너비에 영향을 받지 않기 위해, 카메라 높이로 보정합니다.
	const FVector CamLocation = PlayerCameraManager->GetCameraLocation();
	const FVector StartPoint  = FVector(ScreenWorldLocation.X, ScreenWorldLocation.Y, CamLocation.Z);
	const FVector EndPoint    = StartPoint + (ScreenWorldDirection * Officer->GetSpringArmLength() * 2.0);

	GetWorld()->LineTraceMultiByChannel(OutHitResults, StartPoint, EndPoint, TraceChannel);
}

void AOmnibusPlayerController::LeftButton(const FInputActionValue& InputValue)
{
	EOmniPlayMode PlayMode = GetOmniGameInstance()->GetOmnibusPlayData()->GetPlayMode();
	// if (PlayMode == EOmniPlayMode::Move)
	if (InputValue.Get<bool>() == false)
	{
		AOmniStationBusStop* ResultBusStop = nullptr;
		TArray<FHitResult> HitResults;
		GetMultiHitResultsUnderCursorOnOrthographic(EOmniCollisionChannel::OverlapOnlyTrace, HitResults);
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (IsValid(HitActor) && HitActor->IsA(AOmniStationBusStop::StaticClass()))
			{
				ResultBusStop = Cast<AOmniStationBusStop>(HitActor);
				break;
			}
		}
		if (IsValid(ResultBusStop) == false)
			return;


		if (StartBusStop.IsValid())
		{
			if (StartBusStop.Get() == ResultBusStop || StartBusStop->GetOwnerOmniRoad() == ResultBusStop->GetOwnerOmniRoad())
			{
				// todo: 적절한 알림 메시지 필요.
				UT_LOG("Too Close Between BusStops!")
				StartBusStop = nullptr;
				return;
			}

			// 버스 노선 소환
			const TSubclassOf<AOmniLineBusRoute> BusRouteClass = GetOmniGameInstance()->GetOmnibusPlayData()->GetOmniLineBusRouteClass();
			const FTransform SpawnTransform                    = FTransform(StartBusStop.Get()->GetActorLocation() * FVector(1.0, 1.0, 0.0));
			FActorSpawnParameters SpawnParameter               = FActorSpawnParameters();
			SpawnParameter.SpawnCollisionHandlingOverride      = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AOmniLineBusRoute* NewRoute = GetWorld()->SpawnActor<AOmniLineBusRoute>(BusRouteClass, SpawnTransform, SpawnParameter);
			NewRoute->InitRoutePathAndBus({StartBusStop, ResultBusStop});

			StartBusStop = nullptr;
		}
		else
		{
			StartBusStop = ResultBusStop;
		}
	}
}

void AOmnibusPlayerController::RightButton(const FInputActionValue& InputValue)
{
	const EOmniPlayMode PlayMode = GetOmniGameInstance()->GetOmnibusPlayData()->GetPlayMode();

	if (PlayMode == EOmniPlayMode::Move)
	{
		AOmniOfficerPawn* const Officer = GetPawn<AOmniOfficerPawn>();
		if (UT_IS_VALID(Officer))
		{
			const bool bActive = InputValue.Get<bool>();
			Officer->SetDragActive(bActive, GetMousePosVector2D());
			if (bActive)
				SetInputModeGameOnly();
			else
				SetInputModeGameUI();
		}
	}
}

void AOmnibusPlayerController::Drag(const FInputActionValue& InputValue)
{
	const EOmniPlayMode PlayMode = GetOmniGameInstance()->GetOmnibusPlayData()->GetPlayMode();

	if (PlayMode == EOmniPlayMode::Move)
	{
		AOmniOfficerPawn* const Officer = GetPawn<AOmniOfficerPawn>();
		if (UT_IS_VALID(Officer))
			Officer->DragMap(GetMousePosVector2D());
	}
}

void AOmnibusPlayerController::ToggleTimeStartPause(const FInputActionValue& InputValue)
{
	GetOmniGameInstance()->GetOmniTimeManager()->ToggleStartPauseGameTime();
}

void AOmnibusPlayerController::SpeedUpDownGameTime(const FInputActionValue& InputValue)
{
	const float Axis    = InputValue.Get<float>();
	UOmniTimeManager* TimeManager = GetOmniGameInstance()->GetOmniTimeManager();

	if(Axis < 0.0f)
	{
		TimeManager->GameSpeedDown();
	}
	else if(Axis > 0.0f)
	{
		TimeManager->GameSpeedUp();
	}
}

void AOmnibusPlayerController::ToggleRouteVisibility(const FInputActionValue& InputValue)
{
	if (InputValue.Get<bool>() == false)
	{
		if (AOmnibusRoadManager* RoadManager = GetOmniGameInstance()->GetOmnibusRoadManager())
		{
			RoadManager->ToggleRoutesRender();
		}
	}
}

FVector2D AOmnibusPlayerController::GetMousePosVector2D() const
{
	FVector2D MousePos;
	GetMousePosition(MousePos.X, MousePos.Y);
	return MousePos;
}

UOmnibusGameInstance* AOmnibusPlayerController::GetOmniGameInstance() const
{
	return GetGameInstance<UOmnibusGameInstance>();
}

void AOmnibusPlayerController::SetInputModeGameOnly()
{
	const FInputModeGameOnly InputModeGameOnly = FInputModeGameOnly();
	SetInputMode(InputModeGameOnly);
	SetShowMouseCursor(true);
}

void AOmnibusPlayerController::SetInputModeGameUI()
{
	FInputModeGameAndUI InputModeGameUI = FInputModeGameAndUI();
	InputModeGameUI.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeGameUI);
	SetShowMouseCursor(true);
}
