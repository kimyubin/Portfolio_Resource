// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "EnochGameModeBase.h"
#include "EnochActorFactory.h"
#include "EnochGameInstance.h"
#include "EnochMouseController.h"
#include "Kismet/GameplayStatics.h"


AEnochGameModeBase::AEnochGameModeBase()
{	
	PlayerControllerClass = AEnochMouseController::StaticClass();	//마우스컨트롤러
	
}

void AEnochGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//데이터 초기화. 성공여부 실패하면 PIE 강제 종료
	if (!Cast<UEnochGameInstance>(GetGameInstance())->InitData())
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	
	SubBeginPlay();
	
	ChangeMenuWidget(StartWidgetClass);
}
void AEnochGameModeBase::SubBeginPlay(){;}


void AEnochGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
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