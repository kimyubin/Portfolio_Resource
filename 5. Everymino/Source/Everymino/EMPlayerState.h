// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EMPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class EVERYMINO_API AEMPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AEMPlayerState();

	/** 기존 점수에 추가 점수를 더함.*/
	void AddScore(uint64 InAddScore);

	uint64 GetScore(){return CurrentScore;}

private:
	uint64 CurrentScore;
	
};
