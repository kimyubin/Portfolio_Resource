// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusPlayerController.h"
#include "InputMappingContext.h"

#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "OmnibusGameInstance.h"
#include "OmnibusInputConfig.h"
#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmnibusUtilities.h"
#include "OmniLineBusRoute.h"
#include "OmniOfficerPawn.h"


void AOmnibusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputModeGameUI();

	bEnableTouchEvents     = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents     = true;
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
	EnhancedInputComponent->BindAction(InputActionsConfig->Omni_IA_ToggleRouteVisibility, ETriggerEvent::Triggered, this, &AOmnibusPlayerController::ToggleRouteVisibility);
}

void AOmnibusPlayerController::LeftButton(const FInputActionValue& InputValue)
{
	EOmniPlayMode PlayMode = GetOmniGameInstance()->GetOmnibusPlayData()->GetPlayMode();
	// if (PlayMode == EOmniPlayMode::Move)
	if (InputValue.Get<bool>() == false)
	{
		const TSubclassOf<AOmniLineBusRoute> BusRouteClass = GetOmniGameInstance()->GetOmnibusPlayData()->GetOmniLineBusRouteClass();

		FHitResult Result;
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Result);
		FTransform SpawnTransform   = FTransform(FRotator(), Result.Location * FVector(1.0, 1.0, 0.0));
		AOmniLineBusRoute* NewRoute = GetWorld()->SpawnActorDeferred<AOmniLineBusRoute>(BusRouteClass, SpawnTransform, nullptr, nullptr
		                                                                              , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		NewRoute->MakeRouteAndBus();
		NewRoute->FinishSpawning(SpawnTransform);
	}
}

void AOmnibusPlayerController::RightButton(const FInputActionValue& InputValue)
{
	const EOmniPlayMode PlayMode = GetOmniGameInstance()->GetOmnibusPlayData()->GetPlayMode();

	if (PlayMode == EOmniPlayMode::Move)
	{
		AOmniOfficerPawn* const Officer = GetPawn<AOmniOfficerPawn>();
		if (OB_IS_VALID(Officer))
		{
			const bool bActive = InputValue.Get<bool>();;
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
		if (OB_IS_VALID(Officer))
			Officer->DragMap(GetMousePosVector2D());
	}
}

void AOmnibusPlayerController::ToggleRouteVisibility(const FInputActionValue& InputValue)
{
	AOmnibusRoadManager* RoadManager = GetOmniGameInstance()->GetOmnibusRoadManager();
	if(IsValid(RoadManager))
	{
		const bool bReleased = (InputValue.Get<bool>() == false);
		if(bReleased)
			RoadManager->ToggleRoutesRender();
	}
}

void AOmnibusPlayerController::SpawnAndTrackPreviewBusStop(EBusStopType PreviewType)
{
	if (PreviewType == EBusStopType::None)
		return;

	if (IsPreviewMode)
		CancelBuild();

	IsPreviewMode = true;

	FHitResult Result;
	GetHitResultUnderCursor(ECC_Visibility, false, Result);
}

void AOmnibusPlayerController::SpawnBusStop()
{
	if (bHoverUI)
		return;

	if (IsPreviewMode == false)
		return;

	if (PC_Weak_PreviewBusStop.IsValid())
	{
	}
}

void AOmnibusPlayerController::CancelBuild()
{
	IsPreviewMode = false;
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
