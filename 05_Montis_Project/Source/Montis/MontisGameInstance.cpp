// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MontisGameInstance.h"

#include "MontisGameModeBase.h"
#include "MontisGameUIsHandler.h"
#include "MontisPlayData.h"


UMontisGameInstance::UMontisGameInstance()
{
	MontisPlayDataClass = UMontisPlayData::StaticClass();
	
}

void UMontisGameInstance::Init()
{
	Super::Init();
	GetMontisPlayData();
}

void UMontisGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UMontisGameInstance::LevelInitializer()
{
	const auto GameMode = Cast<AMontisGameModeBase>(GetWorld()->GetAuthGameMode());

	MontisGameUIsHandler = NewObject<UMontisGameUIsHandler>(this, UMontisGameUIsHandler::StaticClass());
		
}
void UMontisGameInstance::LevelDisposer()
{
	
}


UMontisPlayData* UMontisGameInstance::GetMontisPlayData()
{
	if(IsValid(MontisPlayData))
		return MontisPlayData;

	MontisPlayData = NewObject<UMontisPlayData>(this, MontisPlayDataClass);
	return MontisPlayData;
}

UMontisGameUIsHandler* UMontisGameInstance::GetMontisGameUIsHandler()
{
	return MontisGameUIsHandler;
}
