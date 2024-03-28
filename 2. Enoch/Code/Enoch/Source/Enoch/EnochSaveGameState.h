// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/EnochCommonEnums.h"
#include "GameFramework/SaveGame.h"
#include "EnochSaveGameState.generated.h"

// todo 현재 게임 진행 상황 저장용. commanderWrapper에서 호출. 추후 서버에서 데이터를 불러올 예정이기 때문에 커맨더 래퍼에 연결. 서버 연결되면 저장이 아니라 서버에서 호출되는 형태가 될 예정
 
/**
 * 
 */
UCLASS()
class ENOCH_API UEnochSaveGameState : public USaveGame
{
	GENERATED_BODY()
public:
	UEnochSaveGameState();

	UPROPERTY()
	/** 활성화된 커맨더 숫자*/
	int8 ActiveCommanderNum;
	UPROPERTY()
	/** 커맨더별 게임 진행상황*/
	TArray<EGameProgress> SaveGameProgress;
	
	UPROPERTY()
	/** 커맨더 별 도전 중인 층*/
	TArray<int> ChallengedFloor;
	
	void SetSaveData();
	void GetLoadData();
};
