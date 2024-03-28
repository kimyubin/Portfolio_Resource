// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFieldSaveSlot.h"

#include "EnochGameInstance.h"
#include "EnochSaveField.h"
#include "Kismet/GameplayStatics.h"

void UEnochFieldSaveSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UEnochFieldSaveSlot::SaveButtonDown()
{		
	UEnochSaveField* SaveObj = NewObject<UEnochSaveField>();
	SaveObj->SetSaveData(SlotNum, Cast<UEnochGameInstance>(GetGameInstance()));	
}
void UEnochFieldSaveSlot::EnemyLoadButtonDown()
{	
	FString SaveName = FString(("Field_" + to_string(SlotNum)).c_str());
	auto LoadObj = Cast<UEnochSaveField>(UGameplayStatics::LoadGameFromSlot(SaveName,0));
	if(LoadObj!=nullptr)
		LoadObj->GetLoadDataToEnemy(Cast<UEnochGameInstance>(GetGameInstance()));
}

void UEnochFieldSaveSlot::FriendLoadButtonDown()
{
	FString SaveName = FString(("Field_" + to_string(SlotNum)).c_str());
	auto LoadObj = Cast<UEnochSaveField>(UGameplayStatics::LoadGameFromSlot(SaveName,0));
	if(LoadObj!=nullptr)
		LoadObj->GetLoadDataToFriend(Cast<UEnochGameInstance>(GetGameInstance()));
}

