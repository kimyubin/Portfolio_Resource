// Fill out your copyright notice in the Description page of Project Settings.


#include "OmnibusLevelManager.h"

#include "OmnibusGameInstance.h"
#include "OmnibusRoadManager.h"
#include "OmniPassenger.h"
#include "OmniTimeManager.h"

AOmnibusLevelManager::AOmnibusLevelManager()
{
}

void AOmnibusLevelManager::Initialize(AOmnibusRoadManager* InRoadManager)
{
	RoadManager = InRoadManager;
}

void AOmnibusLevelManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AOmnibusLevelManager::BeginPlay()
{
	Super::BeginPlay();

	// 승객 투입
	GetWorldTimerManager().SetTimer(PassengerGenerateHandle, FTimerDelegate::CreateUObject(this, &AOmnibusLevelManager::GeneratePassenger), GetIntervalSec_GeneratePassenger(), true);

	// 신규 블록 개방
	GetWorldTimerManager().SetTimer(BlockOpenHandle, FTimerDelegate::CreateUObject(this, &AOmnibusLevelManager::OpenCityBlock), GetIntervalSec_OpenCityBlock(), true);

	// 블록 타입 변경
	GetWorldTimerManager().SetTimer(BlockTypeChangeHandle, FTimerDelegate::CreateUObject(this, &AOmnibusLevelManager::ChangeCityBlockType), GetIntervalSec_ChangeCityBlockType(), true);
}

void AOmnibusLevelManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(PassengerGenerateHandle);
	GetWorldTimerManager().ClearTimer(BlockOpenHandle);
	GetWorldTimerManager().ClearTimer(BlockTypeChangeHandle);
	
	Super::EndPlay(EndPlayReason);
}

void AOmnibusLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmnibusLevelManager::GeneratePassenger()
{
	// 생성할 승객 수
	constexpr int GenerateNumber = 10;

	for (int PsgCount = 0; PsgCount < GenerateNumber; ++PsgCount)
	{
		auto [HomeInfo, DestInfoList] = RoadManager->GetHomeAndDests();
		AOmniPassenger* Passenger     = RoadManager->PopPassengerInPool();
		Passenger->InitStartJourney(HomeInfo, DestInfoList);
	}
}

void AOmnibusLevelManager::OpenCityBlock()
{
}

void AOmnibusLevelManager::ChangeCityBlockType()
{
}

float AOmnibusLevelManager::GetIntervalSec_GeneratePassenger() const
{
	return GetOmniTimerManager()->GetPassengerGenerateRate() * GetOmniTimerManager()->GetSecInDay();
}

float AOmnibusLevelManager::GetIntervalSec_OpenCityBlock() const
{
	return GetOmniTimerManager()->GetCityBlockOpenRate() * GetOmniTimerManager()->GetSecInDay();
}

float AOmnibusLevelManager::GetIntervalSec_ChangeCityBlockType() const
{
	return GetOmniTimerManager()->GetCityBlockTypeChangeRate() * GetOmniTimerManager()->GetSecInDay();
}

UOmniTimeManager* AOmnibusLevelManager::GetOmniTimerManager() const
{
	return GetGameInstance<UOmnibusGameInstance>()->GetOmniTimeManager();
}
