// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuTypes.h"
#include "KuUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

class UListView;
class UItemSlotButtonUI;
class UImage;
class UWrapBox;
/**
 * 
 */
UCLASS()
class KKUMIGI_API UInventoryUI : public UKuUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	/**
	 * 아이템 리스트 타일뷰를 초기화합니다.
	 */
	void InitItemList();

public:
	void ToggleVisibility();
	bool IsInventoryVisible() const { return bIsVisible; }

protected:
	/** 아이템 리스트 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, EntryClass = UItemSlotButtonUI))
	UListView* ItemListTileView;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* BgImage;

	bool bIsVisible;
};
