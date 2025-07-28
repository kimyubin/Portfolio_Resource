// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuObject.h"
#include "KuUIManager.generated.h"

class UItemSlotButtonUI;
class UKuUserWidget;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class KKUMIGI_API UKuUIManager : public UKuObject
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void ChangeWidget(TSubclassOf<UKuUserWidget> NewWidgetClass);

	void SwitchToPlayMainUI();

	void ToggleInventory() const;
	bool IsInventoryOpen() const;

	TSubclassOf<UItemSlotButtonUI> GetItemSlotButtonClass() { return ItemSlotButtonClass; };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG")
	TSubclassOf<UKuUserWidget> PlayMainWidgetClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemSlotButtonUI> ItemSlotButtonClass;

protected:
	UPROPERTY()
	TObjectPtr<UKuUserWidget> CurrentWidget;
};
