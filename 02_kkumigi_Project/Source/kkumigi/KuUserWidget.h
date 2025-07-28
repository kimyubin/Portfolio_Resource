// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KuUserWidget.generated.h"

class UKuUIManager;
class UItemManager;
class UKuSaveManager;
class UKuPlayData;
class UKuGameInstance;
/**
 * 
 */
UCLASS()
class KKUMIGI_API UKuUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/** 버튼 클릭시 MouseLeave되는 거 방지*/
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** 버튼 클릭시 MouseLeave되는 거 방지*/
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UKuGameInstance* GetKuGameInstance() const;

	UKuPlayData* GetKuPlayData() const;
	UKuSaveManager* GetKuSaveManager() const;
	UItemManager* GetItemManager() const;
	UKuUIManager* GetKuUIManager() const;	
};
