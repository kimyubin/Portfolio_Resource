// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusRoadManager.h"

#include "OmnibusGameInstance.h"
#include "OmnibusUtilities.h"
#include "OmniCityBlock.h"
#include "OmniLineBusRoute.h"
#include "OmniRoadDefaultTwoLane.h"
#include "OmniStationBusStop.h"
#include "Kismet/GameplayStatics.h"


AOmnibusRoadManager::AOmnibusRoadManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bRouteVisible = true;
}

void AOmnibusRoadManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ExecuteAutoManageFunction();
}

void AOmnibusRoadManager::BeginPlay()
{
	Super::BeginPlay();

	GetGameInstance<UOmnibusGameInstance>()->SetOmnibusRoadManager(this);
}

void AOmnibusRoadManager::PostBeginPlay()
{
	Super::PostBeginPlay();

	ExecuteAutoManageFunction();

	for(auto& RouteTuple : OmniRouteTMap)
	{
		AOmniLineBusRoute* Route = RouteTuple.Value.Get();
		if(IsValid(Route) == false)
			continue;

		Route->MakeRouteAndBus();
	}
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

void AOmnibusRoadManager::ExecuteAutoManageFunction()
{
	CollectOmniRoad();
	ExecuteAllRoadsDetect();
	CleanupInvalidStationInRoad();

	CollectStation();
	ExecuteAllStationsDetect();

	CollectCityBlock();
	CleanupInvalidStationInCityBlock();

	CollectRoute();
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

void AOmnibusRoadManager::ExecuteAllRoadsDetect()
{
	for (const auto& RoadTuple : OmniRoadsTMap)
	{
		RoadTuple.Value->DetectAllConnectedOmniRoad();
	}
}

void AOmnibusRoadManager::CleanupInvalidStationInRoad()
{
	for (const auto& RoadTuple : OmniRoadsTMap)
	{
		RoadTuple.Value->RemoveInvalidBusStop();
	}
}

void AOmnibusRoadManager::CollectStation()
{
	OmniStationTMap.Empty();
	TArray<AActor*> StationList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniStationBusStop::StaticClass(), StationList);

	for (AActor* const& RoadListActor : StationList)
	{
		if (RoadListActor->IsA(AOmniStationBusStop::StaticClass()))
			AddStation(Cast<AOmniStationBusStop>(RoadListActor));
	}
}

void AOmnibusRoadManager::ExecuteAllStationsDetect()
{
	for (const auto& Station : OmniStationTMap)
	{
		Station.Value->SearchRoadAndBlock();
	}
}

void AOmnibusRoadManager::CollectCityBlock()
{
	OmniCityBlockTMap.Empty();
	TArray<AActor*> CityBlockList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniCityBlock::StaticClass(), CityBlockList);

	for (AActor* const& RoadListActor : CityBlockList)
	{
		if (RoadListActor->IsA(AOmniCityBlock::StaticClass()))
			AddCityBlock(Cast<AOmniCityBlock>(RoadListActor));
	}
}

void AOmnibusRoadManager::CleanupInvalidStationInCityBlock()
{
	for (const auto& CityBlockTuple : OmniCityBlockTMap)
	{
		CityBlockTuple.Value->RemoveInvalidBusStop();
	}
}

void AOmnibusRoadManager::CollectRoute()
{
	OmniRouteTMap.Empty();
	TArray<AActor*> OmniRouteList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniLineBusRoute::StaticClass(), OmniRouteList);

	for (AActor* const& RoadListActor : OmniRouteList)
	{
		if (RoadListActor->IsA(AOmniLineBusRoute::StaticClass()))
			AddOmniRoute(Cast<AOmniLineBusRoute>(RoadListActor));
	}
}

void AOmnibusRoadManager::AddOmniRoad(AOmniRoad* InRoad)
{
	if (OB_IS_VALID(InRoad) == false)
		return;

	TWeakObjectPtr<AOmniRoad>& RoadMapValue = OmniRoadsTMap.FindOrAdd(InRoad->GetOmniID());
	RoadMapValue = InRoad;
}

void AOmnibusRoadManager::RemoveOmniRoadByID(const uint64 InId)
{
	OmniRoadsTMap.Remove(InId);
}

void AOmnibusRoadManager::RemoveOmniRoad(AOmniRoad* InRoad)
{
	if (OB_IS_VALID(InRoad) == false)
		return;

	RemoveOmniRoadByID(InRoad->GetOmniID());
}

AOmniRoad* AOmnibusRoadManager::FindOmniRoad(const uint64 InId)
{
	const TWeakObjectPtr<AOmniRoad>* FindPtr = OmniRoadsTMap.Find(InId);
	if (FindPtr == nullptr)
		return nullptr;
	else
		return (*FindPtr).Get();
}

void AOmnibusRoadManager::AddStation(AOmniStationBusStop* InStation)
{
	if (OB_IS_VALID(InStation) == false)
		return;

	TWeakObjectPtr<AOmniStationBusStop>& StationMapValue = OmniStationTMap.FindOrAdd(InStation->GetOmniID());
	StationMapValue = InStation;
}

void AOmnibusRoadManager::RemoveStationByID(const uint64 InId)
{
	OmniStationTMap.Remove(InId);
}

void AOmnibusRoadManager::RemoveStation(AOmniStationBusStop* InStation)
{
	if (OB_IS_VALID(InStation) == false)
		return;

	RemoveStationByID(InStation->GetOmniID());
}

AOmniStationBusStop* AOmnibusRoadManager::FindStation(const uint64 InId)
{
	const TWeakObjectPtr<AOmniStationBusStop>* FindPtr = OmniStationTMap.Find(InId);
	if (FindPtr == nullptr)
		return nullptr;
	else
		return (*FindPtr).Get();
}

void AOmnibusRoadManager::AddCityBlock(AOmniCityBlock* InCityBlock)
{
	if (OB_IS_VALID(InCityBlock) == false)
		return;

	TWeakObjectPtr<AOmniCityBlock>& CityBlockMapValue = OmniCityBlockTMap.FindOrAdd(InCityBlock->GetOmniID());
	CityBlockMapValue = InCityBlock;
}

void AOmnibusRoadManager::RemoveCityBlockByID(const uint64 InId)
{
	OmniCityBlockTMap.Remove(InId);
}

void AOmnibusRoadManager::RemoveCityBlock(AOmniCityBlock* InCityBlock)
{
	if (OB_IS_VALID(InCityBlock) == false)
		return;

	RemoveCityBlockByID(InCityBlock->GetOmniID());
}

AOmniCityBlock* AOmnibusRoadManager::FindCityBlock(const uint64 InId)
{
	const TWeakObjectPtr<AOmniCityBlock>* FindPtr = OmniCityBlockTMap.Find(InId);
	if (FindPtr == nullptr)
		return nullptr;
	else
		return (*FindPtr).Get();
}

void AOmnibusRoadManager::AddOmniRoute(AOmniLineBusRoute* InOmniRoute)
{
	if (OB_IS_VALID(InOmniRoute) == false)
		return;

	TWeakObjectPtr<AOmniLineBusRoute>& RouteMapValue = OmniRouteTMap.FindOrAdd(InOmniRoute->GetOmniID());
	RouteMapValue = InOmniRoute;
}

void AOmnibusRoadManager::RemoveOmniRouteByID(const uint64 InId)
{
	OmniRouteTMap.Remove(InId);
}

void AOmnibusRoadManager::RemoveOmniRoute(AOmniLineBusRoute* InOmniRoute)
{
	if (OB_IS_VALID(InOmniRoute) == false)
		return;

	RemoveOmniRouteByID(InOmniRoute->GetOmniID());
}

AOmniLineBusRoute* AOmnibusRoadManager::FindOmniRoute(const uint64 InId)
{
	const TWeakObjectPtr<AOmniLineBusRoute>* FindPtr = OmniRouteTMap.Find(InId);
	if (FindPtr == nullptr)
		return nullptr;
	else
		return (*FindPtr).Get();
}

void AOmnibusRoadManager::SetRoutesRender(const bool SetVisibility)
{
	bRouteVisible = SetVisibility;
	for (auto& RouteTuple : OmniRouteTMap)
	{
		AOmniLineBusRoute* Route = RouteTuple.Value.Get();
		if (IsValid(Route))
			Route->SetRouteRender(SetVisibility);
	}
}

void AOmnibusRoadManager::ToggleRoutesRender()
{
	SetRoutesRender(!bRouteVisible);
}

bool AOmnibusRoadManager::IsRouteVisible() const
{
	return bRouteVisible;
}
