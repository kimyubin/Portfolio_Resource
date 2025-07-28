// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "../Common/EnochFreeLancerData.h"

#include "FLInfoImp.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UFLInfoImp : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
    void UpdateFLInfo();

	UFUNCTION(BlueprintCallable)
    void SetFLSN(int32 SN);

	UFUNCTION(BlueprintCallable)
    void SetFLID(uint8 ID, uint8 level);

	UFUNCTION(BlueprintCallable)
    void Reset();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 SelectedSN;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 SelectedID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 SelectedLevel;	// 0 ~ 4
	
	UPROPERTY(meta = (BindWidget))
	UImage* SlotImg;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextBlock* txtStar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TB_Alliance;
	
	//------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString strHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString strMP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float floatHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float floatMP;

	//------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DPS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString APS;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Range;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString PDef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MDef;

	//------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DmgBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DmgAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ASBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ASAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MSBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MSPercent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Reduce;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ArmBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ArmAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MRBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MRAdd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetSwitcher* switcher;
	
};
