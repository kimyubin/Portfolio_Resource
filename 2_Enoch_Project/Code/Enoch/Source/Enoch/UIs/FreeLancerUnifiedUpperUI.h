// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enoch/EnochFreeLancer.h"

#include "FreeLancerUnifiedUpperUI.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UFreeLancerUnifiedUpperUI : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void SetOwnerFreeLancer(AEnochFreeLancer* InFL);

private:
	TWeakObjectPtr<AEnochFreeLancer> OwnerFreeLancer;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HPProgressBar;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* MPProgressBar;
	UPROPERTY( meta = ( BindWidget ) )
	class UTextBlock* textName;
	
};
