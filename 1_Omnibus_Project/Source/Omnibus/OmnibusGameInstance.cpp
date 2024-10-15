// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusGameInstance.h"

#include "OmniAsync.h"
#include "Omnibus.h"
#include "OmnibusUIsHandler.h"
#include "OmnibusUtilities.h"

#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmniTimeManager.h"
#include "Kismet/KismetSystemLibrary.h"

FOmniWorldDelegates::FOnLevelInitialize FOmniWorldDelegates::OnLevelInitialize;
FOmniWorldDelegates::FOnLevelUninitialize FOmniWorldDelegates::OnLevelUninitialize;


UOmnibusGameInstance::UOmnibusGameInstance()
{
	OmnibusPlayDataClass = UOmnibusPlayData::StaticClass();

	OmnibusPlayData    = nullptr;
	OmnibusUIsHandler  = nullptr;
	OmnibusRoadManager = nullptr;
	OmniPathFinder     = nullptr;
	OmniTimeManager    = nullptr;
}

void UOmnibusGameInstance::Init()
{
	Super::Init();

	OmnibusPlayData = NewObject<UOmnibusPlayData>(this, OmnibusPlayDataClass);
	OmnibusPlayData->Initialize();

	OmnibusUIsHandler = NewObject<UOmnibusUIsHandler>(this, UOmnibusUIsHandler::StaticClass());

	OmniPathFinder = NewObject<UOmniPathFinder>(this);
	OmniPathFinder->Initialize();

	OmniTimeManager = NewObject<UOmniTimeManager>(this);
	OmniTimeManager->Initialize(this);
}

void UOmnibusGameInstance::Shutdown()
{
	// PIE 종류, 레벨 전환 등의 상황에서 데이터 초기화 및 Non-GameThread 작업 제거
	FOmniAsync::ClearDataMapAsync();

	Super::Shutdown();
}

void UOmnibusGameInstance::LevelInitializer()
{
	if (OB_IS_VALID(GetWorld()) == false)
		return;

	if (OmnibusRoadManager.IsValid() == false)
	{
		const FString ErrorMsg = "Error. The RoadManager is not spawned.";
		OB_ERROR("로드 매니저가 스폰되어져 있지 않습니다. %s", *ErrorMsg)
		OmniMsg::Dialog(ErrorMsg);
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	}

	
	FOmniWorldDelegates::OnLevelInitialize.Broadcast(this);
}

void UOmnibusGameInstance::LevelUninitializer()
{
	FOmniWorldDelegates::OnLevelUninitialize.Broadcast(this);
}

UOmnibusPlayData* UOmnibusGameInstance::GetOmnibusPlayData() const
{
	return OmnibusPlayData;
}

UOmnibusUIsHandler* UOmnibusGameInstance::GetOmnibusUIsHandler() const
{
	return OmnibusUIsHandler;
}

AOmnibusRoadManager* UOmnibusGameInstance::GetOmnibusRoadManager() const
{
	/*ensure(OB_IS_WEAK_PTR_VALID(OmnibusRoadManager))*/
	return OmnibusRoadManager.Get();
}

UOmniPathFinder* UOmnibusGameInstance::GetOmniPathFinder() const
{
	return OmniPathFinder;
}

UOmniTimeManager* UOmnibusGameInstance::GetOmniTimeManager() const
{
	return OmniTimeManager;
}

void UOmnibusGameInstance::SetOmnibusRoadManager(AOmnibusRoadManager* InRoadManager)
{
	OmnibusRoadManager = InRoadManager;
}
