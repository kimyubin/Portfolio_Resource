// Fill out your copyright notice in the Description page of Project Settings.


#include "EMSaveKeySettings.h"

#include "BrickTemplate.h"
#include "Kismet/GameplayStatics.h"

UEMSaveKeySettings::UEMSaveKeySettings()
{
	
}
void UEMSaveKeySettings::SaveKeySettings()
{
	FString SaveName = FString("KeySettings");
	KeySettingData = EBindKey::GetKeys();
	UGameplayStatics::SaveGameToSlot(this, SaveName, 0);
}
void UEMSaveKeySettings::LoadKeySettings()
{
	EBindKey::SetKeys(KeySettingData);
}


