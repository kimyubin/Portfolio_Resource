// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EveryminoGameModeBase.h"

#include "BrickBoardManager.h"
#include "EMController.h"
#include "EMDefaultPawn.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AEveryminoGameModeBase::AEveryminoGameModeBase()
{
	//기본 컨트롤러, 디폴트 폰 지정
	PlayerControllerClass = AEMController::StaticClass();
	DefaultPawnClass = AEMDefaultPawn::StaticClass();

	//기본 컨트롤러, 디폴트 폰 지정. C++을 상속받는 BP
	static ConstructorHelpers::FClassFinder<AEMController> PlayerControllerBPClass(TEXT("/Game/GameManager/BP_EveryminoPlayerController"));
	if (PlayerControllerBPClass.Succeeded())
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<AEMDefaultPawn> EMDefaultPawn(TEXT("/Game/PlayeCharacter/BP_EMDefaultPawn"));
	if (EMDefaultPawn.Succeeded())
	{
		DefaultPawnClass = EMDefaultPawn.Class;
	}
}

void AEveryminoGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	MyBoardManager = GetWorld()->SpawnActor<ABrickBoardManager>(ABrickBoardManager::StaticClass(), FVector(-275.f,0.f,500.f), FRotator::ZeroRotator);
	Cast<AEMController>(UGameplayStatics::GetPlayerController(this,0))->SetMyBoardManager(MyBoardManager);
	SubBeginPlay();

	ChangeMenuWidget(StartWidgetClass);
}

void AEveryminoGameModeBase::SubBeginPlay()
{
	
}


void AEveryminoGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
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
