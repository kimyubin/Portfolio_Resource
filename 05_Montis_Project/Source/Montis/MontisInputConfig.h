// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "MontisInputConfig.generated.h"

/**
 * 
 */
UCLASS()
class MONTIS_API UMontisInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_Move;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_Jump;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_Look;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_Aim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_Shoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_LeftLeg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_RightLeg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Montis_IA_CameraTester;
	
};