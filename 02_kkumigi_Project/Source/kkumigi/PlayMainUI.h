// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuUserWidget.h"
#include "PlayMainUI.generated.h"

class UInventoryUI;
/**
 * 
 */
UCLASS()
class KKUMIGI_API UPlayMainUI : public UKuUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void ToggleInventoryWidget();

	bool IsInventoryVisible() const;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UInventoryUI* BP_InventoryUI;
};
