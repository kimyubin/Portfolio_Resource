// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EMSaveKeySettings.generated.h"

using namespace std;
/**
 * 
 */
UCLASS()
class EVERYMINO_API UEMSaveKeySettings : public USaveGame
{
	GENERATED_BODY()
public:
	UEMSaveKeySettings();

	UPROPERTY()
	TArray<FKey> KeySettingData;

	void SaveKeySettings();
	void LoadKeySettings();
};
