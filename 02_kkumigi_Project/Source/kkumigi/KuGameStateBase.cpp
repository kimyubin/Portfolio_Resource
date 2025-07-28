// Fill out your copyright notice in the Description page of Project Settings.


#include "KuGameStateBase.h"

FKuWorldDelegates::FOnPrevLevelBeginPlay FKuWorldDelegates::OnPrevLevelBeginPlay;
FKuWorldDelegates::FOnPostLevelBeginPlay FKuWorldDelegates::OnPostLevelBeginPlay;
FKuWorldDelegates::FOnLevelUninitialize FKuWorldDelegates::OnLevelUninitialize;


void AKuGameStateBase::HandleBeginPlay()
{
	FKuWorldDelegates::OnPrevLevelBeginPlay.Broadcast();

	Super::HandleBeginPlay();

	FKuWorldDelegates::OnPostLevelBeginPlay.Broadcast();
}

void AKuGameStateBase::HandlePostBeginPlay()
{
	const UWorld* World = GetWorld();

	// BeginPlay 정상 작동 후 발동
	if (World && World->GetBegunPlay())
	{
	}
}

void AKuGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FKuWorldDelegates::OnLevelUninitialize.Broadcast();

	Super::EndPlay(EndPlayReason);
}
