// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "OmnibusInputConfig.generated.h"

/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmnibusInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_LeftButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_RightButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Drag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Point_Zoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_MoveCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Center_Zoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Time_Stop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Time_Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Time_Fast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_Time_Slow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Omni_IA_ToggleRouteVisibility;
};
