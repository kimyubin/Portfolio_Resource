// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuObject.h"
#include "KuTypes.h"
#include "GameFramework/SaveGame.h"
#include "KuSaveGame.generated.h"

/**
 * 저장 로직 매니저
 */
UCLASS()
class KKUMIGI_API UKuSaveManager : public UKuObject
{
	GENERATED_BODY()
public:
	UKuSaveManager();

	virtual void Init() override;
	
	void AsyncSaveField();
	void AsyncLoadField();

	/**
	 * 비동기 필드 데이터 로드 델리게이트에 등록할 함수 입니다.
	 */
	void OnPostLoadField(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	
	void AsyncSaveInventory();
	void AsyncLoadInventory();
	
	/**
	 * 비동기 인벤토리 데이터 로드 델리게이트에 등록할 함수 입니다.
	 */
	void OnPostLoadInventory(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	
private:
	FString FieldSlotName;
	FString InventorySlotName;
};

UCLASS()
class KKUMIGI_API USaveInventory : public USaveGame
{
	GENERATED_BODY()

public:
	USaveInventory();
	UPROPERTY(VisibleAnywhere)
	uint32 SlotIdx;

	UPROPERTY(VisibleAnywhere)
	TArray<FItemInvenSaveData> ItemList;
};


UCLASS()
class KKUMIGI_API USaveField : public USaveGame
{
	GENERATED_BODY()

public:
	USaveField();

	UPROPERTY(VisibleAnywhere)
	uint32 SlotIdx;

	UPROPERTY(VisibleAnywhere)
	TArray<FItemFieldSaveData> ItemList;
};
