// Fill out your copyright notice in the Description page of Project Settings.


#include "OmnibusGameStateBase.h"

#include "OmniActor.h"
#include "OmnibusGameInstance.h"

#include "EngineUtils.h"

void AOmnibusGameStateBase::HandleBeginPlay()
{
	FOmniWorldDelegates::OnPrevLevelBeginPlay.Broadcast();

	Super::HandleBeginPlay();

	// 모든 비긴 플레이 호출 이후 호출.
	HandlePostBeginPlay();

	// 게임 시작 준비 완료된 다음, 레벨 초기화 진행.
	FOmniWorldDelegates::OnPostLevelBeginPlay.Broadcast();
}

void AOmnibusGameStateBase::HandlePostBeginPlay()
{
	const UWorld* World = GetWorld();

	// BeginPlay 정상 작동 후 발동
	if (World && World->GetBegunPlay())
	{
		for (AActor* Actor : FActorRange(World))
		{
			if (Actor && Actor->GetClass()->ImplementsInterface(UOmniActorInterface::StaticClass()))
			{
				Cast<IOmniActorInterface>(Actor)->PostBeginPlay();
			}
		}
	}
}

void AOmnibusGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FOmniWorldDelegates::OnLevelEnd.Broadcast();

	Super::EndPlay(EndPlayReason);
}
