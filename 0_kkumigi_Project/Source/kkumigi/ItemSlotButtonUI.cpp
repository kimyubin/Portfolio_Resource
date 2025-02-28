// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotButtonUI.h"

#include "FieldItemFactory.h"
#include "ItemManager.h"
#include "KuAssetManager.h"
#include "KuGameInstance.h"
#include "KuPlayData.h"
#include "KuTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"


void UItemSlotButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UItemSlotButtonUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (ItemID != INDEX_NONE)
	{
		GetItemManager()->GetFieldItemFactory()->SpawnAsyncPreviewItem(ItemID);
	}

	return Reply.NativeReply;
}

void UItemSlotButtonUI::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// 리스트 아이템 오브젝트로 초기화
	if (UItemSlotButtonUIData* SlotDataObj = Cast<UItemSlotButtonUIData>(ListItemObject))
	{
		InitializeItemSlot(SlotDataObj->GetItemID());
	}
}

void UItemSlotButtonUI::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
}

void UItemSlotButtonUI::InitializeItemSlot(const TItemDefID InItemID)
{
	if (InItemID == INDEX_NONE)
		return;

	ItemID = InItemID;

	// 이미지 비동기 로드
	UKuAssetManager& KuAssetManager = UKuAssetManager::Get();
	FStreamableDelegate OnPostLoad  = FStreamableDelegate::CreateUObject(this, &UItemSlotButtonUI::OnPostLoadItemImage, InItemID);
	KuAssetManager.LoadItemUIImage(InItemID, OnPostLoad);
}

void UItemSlotButtonUI::OnPostLoadItemImage(const TItemDefID InItemID)
{
	UKuAssetManager& KuAssetManager = UKuAssetManager::Get();
	ItemSlotImage->SetBrushFromTexture(KuAssetManager.GetItemUIImage(InItemID));
}

TItemDefID UItemSlotButtonUIData::GetItemID() const
{
	return ItemID;
}

void UItemSlotButtonUIData::SetItemID(const TItemDefID InID)
{
	ItemID = InID;
}
