// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OmniCityBlockWidget.generated.h"

enum class ECityBlockType : uint8;
class UTextBlock;
class UImage;
class AOmniCityBlock;
/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmniCityBlockWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetOwnerCityBlock(AOmniCityBlock* InCityBlock);

	void UpdateTypeState(ECityBlockType InNextType);

protected:
	TWeakObjectPtr<AOmniCityBlock> OwnerCityBlock;
	
	UPROPERTY(meta=(BindWidget))
	UImage* TypeImg;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TypeDescription;

	
};
