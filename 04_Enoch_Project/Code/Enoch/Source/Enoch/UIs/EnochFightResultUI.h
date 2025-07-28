// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/BackgroundBlur.h"
#include "Enoch/Common/EnochSimulator.h"

#include "EnochFightResultUI.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UEnochFightResultUI : public UUserWidget
{	
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	GameResult SimulateResult;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBackgroundBlur* ResultBGBlur;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ResultText;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* BlurAnim;

	UFUNCTION(BlueprintCallable)
	void OnNextButtonDown();
	/** 결과 값 수정*/
	UFUNCTION(BlueprintCallable)
	void SetResult(GameResult InResult);
	
};
