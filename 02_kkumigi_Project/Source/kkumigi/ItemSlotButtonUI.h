// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuObject.h"
#include "KuTypes.h"
#include "SlotButtonUI.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ItemSlotButtonUI.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class KKUMIGI_API UItemSlotButtonUI : public USlotButtonUI, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// ~==================================
	// IUserObjectListEntry interface

	/**
	 * 리스트에 새 항목 개체로 할당될 때 호출됩니다.
	 * 추가될 때, 내부 데이터를 초기화 하는 함수 입니다.
	 */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	/** 선택 상태가 변경되면 호출됩니다. */
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	// ~==================================

public:
	/**
	 * 슬롯이 보유한 아이템 정보를 초기화하고 슬롯을 갱신합니다.
	 * @param InItemID 
	 */
	void InitializeItemSlot(const TItemDefID InItemID);

	/**
	 * 아이템 이미지의 로드가 완료된 후 작동하는 델리게이트용 함수입니다.
	 * 슬롯 이미지를 로드된 아이템 이미지로 교체합니다.
	 * @param InItemID 
	 */
	void OnPostLoadItemImage(const TItemDefID InItemID);

protected:
	TItemDefID ItemID;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemSlotImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CountTextBlock;
};

/** IUserObjectListEntry 인터페이스로 전달될 페이로드 */
UCLASS()
class KKUMIGI_API UItemSlotButtonUIData : public UKuObject
{
	GENERATED_BODY()

public:
	TItemDefID GetItemID() const;
	void SetItemID(const TItemDefID InID);

protected:
	TItemDefID ItemID;	
};
