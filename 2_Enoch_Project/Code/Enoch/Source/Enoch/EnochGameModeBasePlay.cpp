// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnochGameModeBasePlay.h"

#include "EnochActorFactory.h"
#include "EnochField.h"
#include "PlayDefaultPawn.h"
#include "UIs/EnochFightResultUI.h"

AEnochGameModeBasePlay::AEnochGameModeBasePlay()
{
	static ConstructorHelpers::FClassFinder<APlayDefaultPawn> PlayDefaultPawn(TEXT("/Game/UI/PlayDefaultPawnBP"));
	if(PlayDefaultPawn.Succeeded())
		DefaultPawnClass = PlayDefaultPawn.Class;
	
	FightResult = GameResult::UnderSimulated;
}

void AEnochGameModeBasePlay::SubBeginPlay()
{
	auto Eworld = GetWorld();
	//AEnochActorFactory, AEnochField 스폰. 팩토리가 먼저.
	Eworld->SpawnActor<AEnochActorFactory>(AEnochActorFactory::StaticClass(), FVector(-330, 70, 160), FRotator::ZeroRotator);
	Eworld->SpawnActor<AEnochField>(AEnochField::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
}

void AEnochGameModeBasePlay::SetFightResult(const GameResult& InResult)
{
	//전투결과를 틱마다 업데이트 받고, 결과가 나오면 뷰포트에 결과 UI 붙임.
	FightResult = InResult;
	if (FightResult != GameResult::UnderSimulated) // && FightResult!=GameResult::None)
	{
		ResultUI = CreateWidget<UUserWidget>(GetWorld(), ResultWidgetClass);
		if (ResultUI != nullptr)
		{
			Cast<UEnochFightResultUI>(ResultUI)->SetResult(InResult);
			ResultUI->AddToViewport(3);
		}
		FightResult = GameResult::UnderSimulated;
	}
}