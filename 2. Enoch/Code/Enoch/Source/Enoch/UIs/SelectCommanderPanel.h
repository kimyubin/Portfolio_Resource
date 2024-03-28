// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectCommanderPanel.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class ENOCH_API USelectCommanderPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentText;
};
