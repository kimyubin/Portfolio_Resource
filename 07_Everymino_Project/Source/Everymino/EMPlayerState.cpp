// Fill out your copyright notice in the Description page of Project Settings.


#include "EMPlayerState.h"

#include "EMGamePlayStatics.h"

AEMPlayerState::AEMPlayerState()
{
	CurrentScore = 0;
}

void AEMPlayerState::AddScore(uint64 InAddScore)
{
	UEMGamePlayStatics::MaxLimitAdder(CurrentScore,InAddScore);
}
