// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelGameModeBase.h"

#include "CitadelBuildingFactory.h"
#include "CitadelForeverGameInstance.h"
#include "CitadelPlayData.h"
#include "CitadelPlayerController.h"
#include "Blueprint/UserWidget.h"

ACitadelGameModeBase::ACitadelGameModeBase()
{
	PlayerControllerClass = ACitadelPlayerController::StaticClass(); //마우스컨트롤러
	CitadelBuildingFactoryClass = ACitadelBuildingFactory::StaticClass();
	DefaultPawnSpawnLocation = FVector(0.,0.,0.);
	DefaultPawnSpawnRotation = FRotator(0.,45.,0.);	
}

void ACitadelGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	GetGameInstance<UCitadelForeverGameInstance>()->LevelInitializer();	
	ChangeMenuWidget(StartWidgetClass);

}

void ACitadelGameModeBase::SubBeginPlay()
{
}

void ACitadelGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
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
