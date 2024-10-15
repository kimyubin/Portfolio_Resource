// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <vector>

#include "Common/Commander.h"
#include "GameFramework/SaveGame.h"
#include "EnochSaveField.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UEnochSaveField : public USaveGame
{
	GENERATED_BODY()
public:
	UEnochSaveField();
	UPROPERTY()
	int8 HeightSize;
	UPROPERTY()
	int8 WidthSize;

	/**원소를 구조체로하면 저장안되서 각각 저장*/
	UPROPERTY()
	TArray<uint8> FLTemplateIDArray;
	UPROPERTY()
	TArray<uint8> FLLvArray;
	
	void VectorToTArray(const vector<vector<FLSaveData>>& VectorHalf);
	vector<vector<FLSaveData>> TarrayToVector();
	/**세이브할 데이터 셋팅
	 * @param SaveSlotNum 세이브 슬롯 번호
	 * @param EGameInstance UEnochGameInstance 포인터. 세이브 함수 내부에서 호출하기 어려워서 포인터를 넘겨받음.
	 */
	void SetSaveData(int SaveSlotNum, class UEnochGameInstance* EGameInstance);
	/**
	 * 세이브 데이터를 적 필드에 스폰
	 * @param EGameInstance UEnochGameInstance 포인터. 세이브 함수 내부에서 호출하기 어려워서 포인터를 넘겨받음.
	 */
	void GetLoadDataToEnemy(class UEnochGameInstance* EGameInstance);
	/**
	 * 세이브 데이터를 아군 필드에 스폰
	 * @param EGameInstance UEnochGameInstance 포인터. 세이브 함수 내부에서 호출하기 어려워서 포인터를 넘겨받음.
	 */
	void GetLoadDataToFriend(class UEnochGameInstance* EGameInstance);
};
