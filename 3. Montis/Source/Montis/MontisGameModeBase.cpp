// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MontisGameModeBase.h"

#include "Archer.h"
#include "MontisGameInstance.h"
#include "MontisPlayerController.h"
#include "Blueprint/UserWidget.h"

AMontisGameModeBase::AMontisGameModeBase()
{
	PlayerControllerClass = AMontisPlayerController::StaticClass();
	DefaultPawnClass = AArcher::StaticClass();
}

void AMontisGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance<UMontisGameInstance>()->LevelInitializer();
	
	ChangeMenuWidget(StartWidgetClass);
}


void AMontisGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
