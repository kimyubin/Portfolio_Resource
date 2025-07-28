// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI.h"

#include "ItemSlotButtonUI.h"
#include "KuAssetManager.h"
#include "KuPlayData.h"
#include "Components/ListView.h"

void UInventoryUI::NativeConstruct()
{
	Super::NativeConstruct();
	bIsVisible = false;
	InitItemList();

	SetVisibility(bIsVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UInventoryUI::InitItemList()
{
	ItemListTileView->SetScrollbarVisibility(ESlateVisibility::Visible);
	ItemListTileView->ClearListItems();

	UKuAssetManager& KuAssetManager  = UKuAssetManager::Get();
	const TArray<const FItemTableRow*>& TableRows = KuAssetManager.GetSortedTableRows();
	for (const FItemTableRow* ItemRow : TableRows)
	{
		UItemSlotButtonUIData* NewSlotData = NewObject<UItemSlotButtonUIData>(this);
		NewSlotData->SetItemID(ItemRow->ItemDefID);
		ItemListTileView->AddItem(NewSlotData);
	}
}

void UInventoryUI::ToggleVisibility()
{
	bIsVisible = !bIsVisible;
	SetVisibility(bIsVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}
