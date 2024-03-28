// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusRoadManager.h"
#include "OmnibusUtilities.h"
#include "OmniRoadDefaultTwoLane.h"
#include "Kismet/GameplayStatics.h"


AOmnibusRoadManager::AOmnibusRoadManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOmnibusRoadManager::BeginPlay()
{
	Super::BeginPlay();

	// 레벨이 시작할 때, 이미 깔려있는 RoadSpline 수집.
	CollectOmniRoad();
}

void AOmnibusRoadManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OmniRoadsTMap.Empty();
}

void AOmnibusRoadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmnibusRoadManager::CollectOmniRoad()
{
	OmniRoadsTMap.Empty();
	TArray<AActor*> RoadList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniRoad::StaticClass(), RoadList);
	
	for (AActor* const& RoadListActor : RoadList)
	{
		if (RoadListActor->IsA(AOmniRoad::StaticClass()))
			AddOmniRoad(Cast<AOmniRoad>(RoadListActor));
	}
}

void AOmnibusRoadManager::AddOmniRoad(AOmniRoad* InRoad)
{
	OmniContainer::TMap_Emplace(OmniRoadsTMap, InRoad->GetOmniID(), InRoad);
}

void AOmnibusRoadManager::RemoveOmniRoad(const uint64 InId)
{
	OmniRoadsTMap.Remove(InId);
}

void AOmnibusRoadManager::RemoveOmniRoad(AOmniRoad* InRoad)
{
	RemoveOmniRoad(InRoad->GetOmniID());
}

AOmniRoad* AOmnibusRoadManager::FindOmniRoad(const uint64 InId)
{
	return OmniContainer::TMap_Find(OmniRoadsTMap, InId);
}
