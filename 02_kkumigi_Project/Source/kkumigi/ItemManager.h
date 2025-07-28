// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuObject.h"
#include "KuTypes.h"
#include "ItemManager.generated.h"

class AFieldItemFactory;
class FOnlineInventory;
class AFieldItem;

class FInventory
{
public:
	FInventory();
	void AddItemFromField(const AFieldItem* InFieldItem);
	void AddItemFromField(const AFieldItem* InFieldItem, const uint32 InIdx);

	TArray<FItemData> Items;
};

/**
 * 아이템 관련 로직 및 데이터 관리.
 */
UCLASS()
class KKUMIGI_API UItemManager : public UKuObject
{
	GENERATED_BODY()

public:
	UItemManager();
	virtual void Init() override;
	virtual void BeginDestroy() override;

	void PrevLevelBeginPlay();
	void PostLevelBeginPlay();

	/** 필드에 있는 아이템을 인벤토리에 집어넣습니다. */
	void MoveItemFieldToInventory(const AFieldItem* InFieldItem, const int32 InInvenIdx);

	FInventory* GetInventory() const;
	AFieldItemFactory* GetFieldItemFactory() const;
	FOnlineInventory* GetOnlineInventory() const;

private:
	/** 인벤토리에 들어있는 아이템 목록 */
	TUniquePtr<FInventory> Inventory;

	/** 필드 아이템 관리 주체. 필드 아이템 스폰 및 목록 관리. */
	UPROPERTY()
	TWeakObjectPtr<AFieldItemFactory> FieldItemFactory;

	/** 캐싱된 온라인 아이템 목록 */
	TUniquePtr<FOnlineInventory> OnlineInventory;
};
