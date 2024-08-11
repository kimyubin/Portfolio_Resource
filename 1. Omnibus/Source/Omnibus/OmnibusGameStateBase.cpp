// Fill out your copyright notice in the Description page of Project Settings.


#include "OmnibusGameStateBase.h"

#include "OmnibusGameInstance.h"
#include "EngineUtils.h"
#include "OmniActor.h"

void AOmnibusGameStateBase::HandleBeginPlay()
{
	Super::HandleBeginPlay();

	// 모든 비긴 플레이 호출 이후 호출됨.
	HandlePostBeginPlay();

	// 레벨에 있는 모든 액터들의 BeginPlay, PostBeginPlay 호출 완료.(GameMode 포함)
	// 게임 시작 준비 완료된 다음 레벨 초기화 진행.
	GetGameInstance<UOmnibusGameInstance>()->LevelInitializer();
}

void AOmnibusGameStateBase::HandlePostBeginPlay()
{
	const UWorld* World = GetWorld();

	// BeginPlay 정상 작동 후 발동
	if (World && World->GetBegunPlay())
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor->GetClass()->ImplementsInterface(UOmniActorInterface::StaticClass()))
			{
				Cast<IOmniActorInterface>(Actor)->PostBeginPlay();
			}
		}
	}
}

void AOmnibusGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance<UOmnibusGameInstance>()->LevelUninitializer();

	Super::EndPlay(EndPlayReason);
}
