// Fill out your copyright notice in the Description page of Project Settings.

#include "KuGameInstance.h"


#include <steam/isteaminventory.h>

#include "KuSaveGame.h"
#include "OnlineSubsystemUtils.h"
#include "ItemManager.h"
#include "KuAssetManager.h"
#include "KuGameStateBase.h"
#include "KuPlayData.h"
#include "KuUIManager.h"

#include "UtlLog.h"

#include "Kismet/GameplayStatics.h"
#include "Voxel/Chunk/ChunkManager.h"

#include "UTLStatics.h"

UKuGameInstance::UKuGameInstance()
{
	KuPlayDataClass  = nullptr; //UKuPlayData::StaticClass();
	KuUIManagerClass = nullptr;

	PlayData     = nullptr;
	SaveManager  = nullptr;
	ItemManager  = nullptr;
	UIManager    = nullptr;
	ChunkManager = nullptr;
}

void UKuGameInstance::Init()
{
	Super::Init();

	FKuWorldDelegates::OnPrevLevelBeginPlay.AddUObject(this, &UKuGameInstance::PrevLevelBeginPlay);
	FKuWorldDelegates::OnPostLevelBeginPlay.AddUObject(this, &UKuGameInstance::PostLevelBeginPlay);

	GetOnlineSubSystem();

	// BP 클래스 등록 안된 경우 실행 안함.
	UT_IF(KuPlayDataClass == nullptr || KuUIManagerClass == nullptr)
	{
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
		return;
	}

	UKuAssetManager::Get().Init();

	PlayData     = NewObject<UKuPlayData>(this, KuPlayDataClass);
	SaveManager  = NewObject<UKuSaveManager>(this);
	ItemManager  = NewObject<UItemManager>(this);
	UIManager    = NewObject<UKuUIManager>(this, KuUIManagerClass);
	ChunkManager = NewObject<UChunkManager>(this, ChunkManagerClass);

	SaveManager->Init();
	ItemManager->Init();
	UIManager->Init();
	ChunkManager->Init();
}

void UKuGameInstance::Shutdown()
{
	FKuWorldDelegates::OnPrevLevelBeginPlay.RemoveAll(this);
	FKuWorldDelegates::OnPostLevelBeginPlay.RemoveAll(this);
	FKuWorldDelegates::OnPrevLevelBeginPlay.Clear();
	FKuWorldDelegates::OnPostLevelBeginPlay.Clear();

	Super::Shutdown();
}

void UKuGameInstance::PrevLevelBeginPlay()
{
}

void UKuGameInstance::PostLevelBeginPlay()
{
	// BP 클래스 등록 안할 경우 실행 안함.
	UT_IF(KuPlayDataClass == nullptr || KuUIManagerClass == nullptr)
	{
		UtlMsg::Dialog(TEXT("KuPlayDataClass or KuUIManagerClass is not registered."));
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
		return;
	}
	//todo: 임시
	UIManager->SwitchToPlayMainUI();
}

UKuPlayData* UKuGameInstance::GetKuPlayData() const
{
	return PlayData;
}

UKuSaveManager* UKuGameInstance::GetKuSaveManager() const { return SaveManager; }

UItemManager* UKuGameInstance::GetItemManager() const { return ItemManager; }

UKuUIManager* UKuGameInstance::GetUIManager() const { return UIManager; }

UChunkManager* UKuGameInstance::GetChunkManager() const { return ChunkManager; }


void UKuGameInstance::GetOnlineSubSystem()
{
	// OnlineSubsystem 받아오기
	const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		// OnlineSubsystem Interface 받아오기
		const IOnlineSessionPtr& OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		// OnlineSubsystem 이름 출력
		UT_DEBUG_MSG(5, "OnlineSubsystem: %s", *OnlineSubsystem->GetSubsystemName().ToString())
	}
	SteamInventoryResult_t pResultHandle;
	SteamItemDef_t dropListDefinition = 0;

	if (ISteamInventory* SteamInven = SteamInventory())
	{
		bool res = SteamInven->TriggerItemDrop(&pResultHandle, dropListDefinition);
		UT_DEBUG_MSG(10, "%d,%d,%d", res, pResultHandle, dropListDefinition)
		SteamInven->GetAllItems(&pResultHandle);
		UT_DEBUG_MSG(10, "%d,%d,%d", res, pResultHandle, dropListDefinition)
	}
}
