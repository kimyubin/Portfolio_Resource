// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelForeverGameInstance.h"

#include "CitadelBuildingFactory.h"
#include "CitadelDefaultPawn.h"
#include "CitadelGameModeBase.h"
#include "CitadelGameUIsHandler.h"
#include "CitadelPlayData.h"

UCitadelForeverGameInstance::UCitadelForeverGameInstance()
{
	CitadelPlayDataClass = UCitadelPlayData::StaticClass();
	
}

void UCitadelForeverGameInstance::Init()
{
	Super::Init();
	GetCitadelPlayData();
}

void UCitadelForeverGameInstance::Shutdown()
{
	Super::Shutdown();
}
void UCitadelForeverGameInstance::LevelInitializer()
{
	const auto GameMode = Cast<ACitadelGameModeBase>(GetWorld()->GetAuthGameMode());
	
	CitadelDefaultPawn = 
		GetWorld()->SpawnActor<ACitadelDefaultPawn>(GameMode->CitadelControlPawnClass, GameMode->GetDefaultPawnSpawnLocation(), GameMode->GetDefaultPawnSpawnRotation());
	CitadelBuildingFactory =
		GetWorld()->SpawnActor<ACitadelBuildingFactory>(ACitadelBuildingFactory::StaticClass());
	CitadelGameUIsHandler =
		NewObject<UCitadelGameUIsHandler>(this, UCitadelGameUIsHandler::StaticClass());
		
}
void UCitadelForeverGameInstance::LevelDisposer()
{
	
}


UCitadelPlayData* UCitadelForeverGameInstance::GetCitadelPlayData()
{
	if(IsValid(CitadelPlayData))
		return CitadelPlayData;

	CitadelPlayData = NewObject<UCitadelPlayData>(this, CitadelPlayDataClass);
	return CitadelPlayData;
}

ACitadelDefaultPawn* UCitadelForeverGameInstance::GetCitadelDefaultPawn()
{
	return CitadelDefaultPawn;
}

ACitadelBuildingFactory* UCitadelForeverGameInstance::GetCitadelBuildingFactory()
{
	return CitadelBuildingFactory;	
}

UCitadelGameUIsHandler* UCitadelForeverGameInstance::GetCitadelGameUIsHandler()
{
	return CitadelGameUIsHandler;
}
