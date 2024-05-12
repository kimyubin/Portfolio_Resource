// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusGameInstance.h"

#include "Omnibus.h"
#include "OmnibusUIsHandler.h"
#include "OmnibusUtilities.h"

#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "Kismet/KismetSystemLibrary.h"


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

	if (OmnibusRoadManager.IsValid() == false)
	{
		const FString ErrorMsg = "Error. The RoadManager is not spawned.";
		OB_ERROR("로드 매니저가 스폰되어져 있지 않습니다. %s", *ErrorMsg)
		OmniMsg::Dialog(ErrorMsg);
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	}

	OmnibusUIsHandler = NewObject<UOmnibusUIsHandler>(this, UOmnibusUIsHandler::StaticClass());
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

AOmnibusRoadManager* UOmnibusGameInstance::GetOmnibusRoadManager() const
{
	if (OB_IS_WEAK_PTR_VALID(OmnibusRoadManager) == false)
		return nullptr;

	return OmnibusRoadManager.Get();
}

void UOmnibusGameInstance::SetOmnibusRoadManager(AOmnibusRoadManager* InRoadManager)
{
	OmnibusRoadManager = InRoadManager;
}
