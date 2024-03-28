// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enoch/EnochFreeLancer.h"

#include "FreeLancerDeathUI.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UFreeLancerDeathUI : public UUserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* OpacityAnimation;

	void SetOwnerFreeLancer(AEnochFreeLancer* InFL){OwnerFreeLancer = InFL;}
	TWeakObjectPtr<AEnochFreeLancer> OwnerFreeLancer;

	bool isDeathTrigger;
};
