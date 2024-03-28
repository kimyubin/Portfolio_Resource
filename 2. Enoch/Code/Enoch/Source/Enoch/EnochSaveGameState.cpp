// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochSaveGameState.h"

UEnochSaveGameState::UEnochSaveGameState()
{
	ActiveCommanderNum=0;
	SaveGameProgress.Init(EGameProgress::None, 2);
	ChallengedFloor.Init(0,2);	
}