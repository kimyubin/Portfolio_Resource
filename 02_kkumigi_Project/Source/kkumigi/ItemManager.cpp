// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"

#include "FieldItemFactory.h"
#include "KuGameStateBase.h"
#include "SteamInventory.h"

#include "UTLStatics.h"


FInventory::FInventory()
{
}

UItemManager::UItemManager()
{
	Inventory        = nullptr;
	OnlineInventory  = nullptr;
	FieldItemFactory = nullptr;
}

void UItemManager::Init()
{
	Super::Init();

	FKuWorldDelegates::OnPrevLevelBeginPlay.AddUObject(this, &UItemManager::PrevLevelBeginPlay);
	FKuWorldDelegates::OnPostLevelBeginPlay.AddUObject(this, &UItemManager::PostLevelBeginPlay);

	Inventory = MakeUnique<FInventory>();

	if (SteamInventory())
	{
		OnlineInventory = MakeUnique<FSteamLocalInventory>();
	}
#if WITH_EDITOR
	else
	{
		OnlineInventory = MakeUnique<FOnlineLocalInventory>();
	}
#endif //WITH_EDITORONLY_DATA
}

void UItemManager::BeginDestroy()
{
	FKuWorldDelegates::OnPrevLevelBeginPlay.RemoveAll(this);
	FKuWorldDelegates::OnPostLevelBeginPlay.RemoveAll(this);

	Super::BeginDestroy();
}

void UItemManager::PrevLevelBeginPlay()
{
}

void UItemManager::PostLevelBeginPlay()
{
	if (UWorld* World = GetWorld())
	{
		FieldItemFactory = World->SpawnActor<AFieldItemFactory>();
	}

	// todo: 저장 및 온라인 인벤토리를 바탕으로 아이템 스폰 철차 실시.
}

void UItemManager::MoveItemFieldToInventory(const AFieldItem* InFieldItem, const int32 InInvenIdx)
{
}

FInventory* UItemManager::GetInventory() const { return Inventory.Get(); }

AFieldItemFactory* UItemManager::GetFieldItemFactory() const { return FieldItemFactory.Get(); }

FOnlineInventory* UItemManager::GetOnlineInventory() const { return OnlineInventory.Get(); }
