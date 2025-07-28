// Fill out your copyright notice in the Description page of Project Settings.


#include "KuAssetManager.h"

#include "FieldItem.h"
#include "KuTypes.h"

#include "UtlLog.h"

UKuAssetManager& UKuAssetManager::Get()
{
	if (UKuAssetManager* Singleton = Cast<UKuAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
	UT_VERB_LOG(Fatal, "Cannot use AssetManager if no AssetManagerClassName is defined!")
	return *NewObject<UKuAssetManager>();
}

void UKuAssetManager::Init()
{
	if (ItemDataTable == nullptr)
	{
		UT_LOG("ItemDataTable is not defined")
		return;
	}

	// 테이블 캐싱
	CachedSortedTableRows.Empty();
	GetSortedTableRows();

#if WITH_EDITORONLY_DATA
	// Object Path 유효성 검사
	for (const FItemTableRow* ItemRow : CachedSortedTableRows)
	{
		if (ItemRow)
		{
			FString PathStr = ItemRow->ItemActorClass.ToSoftObjectPath().ToString();
			if (PathStr.Right(2).Equals(TEXT("_C"), ESearchCase::IgnoreCase) == false)
			{
				UT_LOG("ItemClass's ObjectPath don't have '_C'. %s", *PathStr)
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

const TArray<const FItemTableRow*>& UKuAssetManager::GetSortedTableRows()
{
	// 캐시가 없다면 가져와서 정렬
	if (CachedSortedTableRows.IsEmpty())
	{
		ItemDataTable->GetAllRows(GetName(), CachedSortedTableRows);

		// ID 오름차순 정렬
		Algo::Sort(CachedSortedTableRows, [](const FItemTableRow* A, const FItemTableRow* B)
		{
			return A->ItemDefID < B->ItemDefID;
		});
	}

	return CachedSortedTableRows;
}

void UKuAssetManager::LoadFieldItem(const TItemDefID InItemID, FStreamableDelegate DelegateToCall)
{
	FItemTableRow* ItemRow    = ItemDataTable->FindRow<FItemTableRow>(FName(FString::FormatAsNumber(InItemID)), GetName());
	FSoftObjectPath ItemClass = ItemRow->ItemActorClass.ToSoftObjectPath();

	LoadAssetList({ItemClass}, MoveTemp(DelegateToCall));
}

void UKuAssetManager::LoadItemUIImage(const TItemDefID InItemID, FStreamableDelegate DelegateToCall)
{
	FItemTableRow* ItemRow           = ItemDataTable->FindRow<FItemTableRow>(FName(FString::FormatAsNumber(InItemID)), GetName());
	FSoftObjectPath ThumbnailTexture = ItemRow->Thumbnail.ToSoftObjectPath();

	LoadAssetList({ThumbnailTexture}, MoveTemp(DelegateToCall));
}

FPrimaryAssetId UKuAssetManager::GetFieldItemPrimaryAssetId(const TItemDefID InItemID) const
{
	FItemTableRow* ItemRow = ItemDataTable->FindRow<FItemTableRow>(FName(FString::FormatAsNumber(InItemID)), GetName());
	if (ItemRow)
	{
		FString ItemName = TEXT("BP_") + ItemRow->ItemSignatureName.ToString();
		return FPrimaryAssetId(AFieldItem::ItemPrimaryAssetType, *ItemName);
	}
	return FPrimaryAssetId();
}

TSubclassOf<AFieldItem> UKuAssetManager::GetFieldItemClass(const TItemDefID InItemID) const
{
	FItemTableRow* ItemRow = ItemDataTable->FindRow<FItemTableRow>(FName(FString::FormatAsNumber(InItemID)), GetName());

	return ItemRow->ItemActorClass.Get();
}

UTexture2D* UKuAssetManager::GetItemUIImage(const TItemDefID InItemID) const
{
	FItemTableRow* ItemRow = ItemDataTable->FindRow<FItemTableRow>(FName(FString::FormatAsNumber(InItemID)), GetName());

	return ItemRow->Thumbnail.Get();
}
