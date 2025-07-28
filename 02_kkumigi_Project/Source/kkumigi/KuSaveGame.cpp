// Fill out your copyright notice in the Description page of Project Settings.


#include "KuSaveGame.h"

#include "KuGameInstance.h"
#include "UtlLog.h"
#include "Kismet/GameplayStatics.h"


UKuSaveManager::UKuSaveManager()
{
	FieldSlotName     = TEXT("FieldSlotName");
	InventorySlotName = TEXT("InventorySlotName");
}

void UKuSaveManager::Init()
{
	Super::Init();
}

void UKuSaveManager::AsyncSaveField()
{
	UKuGameInstance* KuGameInstance = GetKuGameInstance();
	USaveField* SaveGameInstance    = Cast<USaveField>(UGameplayStatics::CreateSaveGameObject(USaveField::StaticClass()));
	if (KuGameInstance == nullptr && SaveGameInstance == nullptr)
	{
		UT_LOG("Fail to Save Field")
		return;
	}

	// ...
	// do something
	// 게임 인스턴스에서 데이터 가져오기
	// KuGameInstance->GetData
	// ...

	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
	AsyncSaveDelegate.BindLambda([](const FString& str, const int32 idx, bool bsuccess)
	{
		if (bsuccess)
			UT_LOG("success")
	});

	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, FieldSlotName, 0, AsyncSaveDelegate);
}

void UKuSaveManager::AsyncLoadField()
{
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindUObject(this, &UKuSaveManager::OnPostLoadField);
	UGameplayStatics::AsyncLoadGameFromSlot(FieldSlotName, 0, LoadDelegate);
}

void UKuSaveManager::OnPostLoadField(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UKuGameInstance* KuGameInstance = GetKuGameInstance();
	USaveField* LoadData            = Cast<USaveField>(LoadedGameData);
	if (KuGameInstance == nullptr && LoadData == nullptr)
	{
		UT_LOG("Fail to Load Field")
		return;
	}

	// ...
	// do something
	// 로드한 데이터 업데이트
	// KuGameInstance->SetData
	// ...
}


void UKuSaveManager::AsyncSaveInventory()
{
	UKuGameInstance* KuGameInstance = GetKuGameInstance();
	USaveInventory* SaveData        = Cast<USaveInventory>(UGameplayStatics::CreateSaveGameObject(USaveInventory::StaticClass()));

	if (KuGameInstance == nullptr && SaveData == nullptr)
	{
		UT_LOG("Fail to Save Inventory")
		return;
	}

	// ...
	// do something
	// 게임 인스턴스에서 데이터 가져와서 저장 슬롯에 넣기
	// SaveInven->SlotIdx
	// SaveInven->ItemList = KuGameInstance->GetData
	// ...

	// 성공/실패 후 호출할 델리게이트 설정(선택)
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindLambda([](const FString& str, const int32 idx, bool bsuccess)
	{
		if (bsuccess)
			UT_LOG("success")
	});

	UGameplayStatics::AsyncSaveGameToSlot(SaveData, InventorySlotName, 0, SaveDelegate);
}

void UKuSaveManager::AsyncLoadInventory()
{
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindUObject(this, &UKuSaveManager::OnPostLoadInventory);
	UGameplayStatics::AsyncLoadGameFromSlot(InventorySlotName, 0, LoadDelegate);
}

void UKuSaveManager::OnPostLoadInventory(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UKuGameInstance* KuGameInstance = GetKuGameInstance();
	USaveInventory* LoadData        = Cast<USaveInventory>(LoadedGameData);
	if (KuGameInstance == nullptr && LoadData == nullptr)
	{
		UT_LOG("Fail to Load USaveInventory")
		return;
	}

	// ...
	// do something
	// 로드한 데이터 업데이트
	// KuGameInstance->SetData
	// ...
}


USaveInventory::USaveInventory()
{
	SlotIdx = 0;

	ItemList = {};
}

USaveField::USaveField()
{
	SlotIdx = 0;

	ItemList = {};
}
