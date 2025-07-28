// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusGameInstance.h"

#include "OmniAsync.h"
#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmnibusUIsHandler.h"
#include "OmniOnlineSubsystem.h"
#include "OmniTimeManager.h"
#include "Kismet/KismetSystemLibrary.h"

#include "UtlLog.h"
#include "UTLStatics.h"

FOmniWorldDelegates::FOnPrevLevelBeginPlay FOmniWorldDelegates::OnPrevLevelBeginPlay;
FOmniWorldDelegates::FOnPostLevelBeginPlay FOmniWorldDelegates::OnPostLevelBeginPlay;
FOmniWorldDelegates::FOnLevelEnd FOmniWorldDelegates::OnLevelEnd;


UOmnibusGameInstance::UOmnibusGameInstance()
{
	OmnibusPlayDataClass = UOmnibusPlayData::StaticClass();

	OmnibusPlayData    = nullptr;
	OmnibusUIsHandler  = nullptr;
	OmnibusRoadManager = nullptr;
	OmniPathFinder     = nullptr;
	OmniTimeManager    = nullptr;

	OmniOnlineSubsystem = nullptr;
}

void UOmnibusGameInstance::Init()
{
	Super::Init();
	PrevLevelBeginPlayHandle = FOmniWorldDelegates::OnPrevLevelBeginPlay.AddUObject(this, &UOmnibusGameInstance::PrevLevelBeginPlay);
	PostLevelBeginPlayHandle = FOmniWorldDelegates::OnPostLevelBeginPlay.AddUObject(this, &UOmnibusGameInstance::PostLevelBeginPlay);
	LevelEndHandle           = FOmniWorldDelegates::OnLevelEnd.AddUObject(this, &UOmnibusGameInstance::LevelEnd);

	OmnibusPlayData     = UOmniObject::NewInitObject<UOmnibusPlayData>(this, OmnibusPlayDataClass);
	OmnibusUIsHandler   = UOmniObject::NewInitObject<UOmnibusUIsHandler>(this, UOmnibusUIsHandler::StaticClass());
	OmniPathFinder      = UOmniObject::NewInitObject<UOmniPathFinder>(this);
	OmniTimeManager     = UOmniObject::NewInitObject<UOmniTimeManager>(this);
	OmniOnlineSubsystem = UOmniObject::NewInitObject<UOmniOnlineSubsystem>(this);
}

void UOmnibusGameInstance::Shutdown()
{
	FOmniWorldDelegates::OnPrevLevelBeginPlay.Remove(PrevLevelBeginPlayHandle);
	FOmniWorldDelegates::OnPostLevelBeginPlay.Remove(PostLevelBeginPlayHandle);
	FOmniWorldDelegates::OnLevelEnd.Remove(LevelEndHandle);

	// PIE 종료, 레벨 전환 등의 상황에서 데이터 초기화 및 Non-GameThread 작업 제거
	FOmniAsync::ClearProxyDataAsync();

	Super::Shutdown();
}

void UOmnibusGameInstance::PrevLevelBeginPlay()
{
}

void UOmnibusGameInstance::PostLevelBeginPlay()
{
	if (UT_IS_VALID(GetWorld()) == false)
		return;

	if (OmnibusRoadManager.IsValid() == false)
	{
		const FString ErrorMsg = "Error. RoadManager is not spawned.";
		UT_ERROR("로드 매니저가 스폰되어져 있지 않습니다. %s", *ErrorMsg)
		UtlMsg::Dialog(ErrorMsg);
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	}
}

void UOmnibusGameInstance::LevelEnd()
{
	
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
	/*ensure(UT_IS_WEAK_PTR_VALID(OmnibusRoadManager))*/
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

UOmniOnlineSubsystem* UOmnibusGameInstance::GetOmnibusOnlineSubsystem() const
{
	return OmniOnlineSubsystem;
}

void UOmnibusGameInstance::SetOmnibusRoadManager(AOmnibusRoadManager* InRoadManager)
{
	OmnibusRoadManager = InRoadManager;
}
