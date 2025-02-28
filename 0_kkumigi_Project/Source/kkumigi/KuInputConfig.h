// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "KuInputConfig.generated.h"

/**
 * 
 */
UCLASS()
class KKUMIGI_API UKuInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IA_WheelAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IA_LeftButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IA_RightButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IA_Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IA_ItemRotate;
};
