// Copyright Epic Games, Inc. All Rights Reserved.


#include "OmnibusGameModeBase.h"

#include "OmnibusGameInstance.h"
#include "OmnibusPlayerController.h"
#include "OmniOfficerPawn.h"
#include "Blueprint/UserWidget.h"

AOmnibusGameModeBase::AOmnibusGameModeBase()
{
	PlayerControllerClass = AOmnibusPlayerController::StaticClass();
	DefaultPawnClass      = AOmniOfficerPawn::StaticClass();
	CurrentWidget         = nullptr;
}

void AOmnibusGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance<UOmnibusGameInstance>()->LevelInitializer();

	ChangeMenuWidget(StartWidgetClass);
}

void AOmnibusGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
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
