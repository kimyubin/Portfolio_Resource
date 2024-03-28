// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "EnochFieldSaveSlot.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UEnochFieldSaveSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int SlotNum;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SaveText;
	
	UFUNCTION(BlueprintCallable)
	void SaveButtonDown();
	UFUNCTION(BlueprintCallable)
	void EnemyLoadButtonDown();
	UFUNCTION(BlueprintCallable)
	void FriendLoadButtonDown();
};
