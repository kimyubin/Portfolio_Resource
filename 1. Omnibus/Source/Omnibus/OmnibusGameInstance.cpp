// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusGameInstance.h"

#include "Omnibus.h"
#include "OmnibusUIsHandler.h"
#include "OmnibusUtilities.h"

#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"


UOmnibusGameInstance::UOmnibusGameInstance()
{
	OmnibusPlayDataClass = UOmnibusPlayData::StaticClass();

	OmnibusPlayData    = nullptr;
	OmnibusUIsHandler  = nullptr;
	OmnibusRoadManager = nullptr;
}

void UOmnibusGameInstance::Init()
{
	Super::Init();
	GetOmnibusPlayData();
}

void UOmnibusGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UOmnibusGameInstance::LevelInitializer()
{
	if (OB_IS_VALID(GetWorld()) == false)
		return;

	FOmniTime::SetLevelStartTime_Sec();

	OmnibusUIsHandler  = NewObject<UOmnibusUIsHandler>(this, UOmnibusUIsHandler::StaticClass());
	OmnibusRoadManager = GetWorld()->SpawnActor<AOmnibusRoadManager>(AOmnibusRoadManager::StaticClass());
}

void UOmnibusGameInstance::LevelDisposer()
{
}

UOmnibusPlayData* UOmnibusGameInstance::GetOmnibusPlayData()
{
	if (IsValid(OmnibusPlayData))
		return OmnibusPlayData;

	OmnibusPlayData = NewObject<UOmnibusPlayData>(this, OmnibusPlayDataClass);
	return OmnibusPlayData;
}

UOmnibusUIsHandler* UOmnibusGameInstance::GetOmnibusUIsHandler()
{
	if (OB_IS_VALID(OmnibusUIsHandler) == false)
		return nullptr;

	return OmnibusUIsHandler;
}

AOmnibusRoadManager* UOmnibusGameInstance::GetOmnibusRoadManager()
{
	if (OB_IS_VALID(OmnibusRoadManager) == false)
		return nullptr;

	return OmnibusRoadManager;
}
