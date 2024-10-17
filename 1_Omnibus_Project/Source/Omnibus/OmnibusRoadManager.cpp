// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusRoadManager.h"

#include "OmniAsync.h"
#include "OmnibusGameInstance.h"
#include "OmnibusLevelManager.h"
#include "OmnibusPlayData.h"
#include "OmniCityBlock.h"
#include "OmniLineBusRoute.h"
#include "OmniPassenger.h"
#include "OmniPlayMainUI.h"
#include "OmniRoadDefaultTwoLane.h"
#include "OmniStationBusStop.h"
#include "PathVisualizator.h"
#include "Kismet/GameplayStatics.h"

#include "UtlLog.h"
#include "UTLStatics.h"


AOmnibusRoadManager::AOmnibusRoadManager()
{
	PrimaryActorTick.bCanEverTick = true;

	PoolCapacity       = 64;
	PoolLowerThreshold = 32;
	bPoolInsufficient  = true;

	LevelManager = nullptr;

	BusRouteMapVersion = 0;

	bPassengerVisible = false;
	bRouteVisible     = false;
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

	PassengerPool = MakeUnique<TCircularQueue<TWeakObjectPtr<AOmniPassenger>>>(PoolCapacity);

	SetActorTickEnabled(true);
}

void AOmnibusRoadManager::PostBeginPlay()
{
	Super::PostBeginPlay();

	ExecuteAutoManageFunction();

	for (const auto& [Key, RouteWeak] : OmniRouteTMap)
	{
		if (AOmniLineBusRoute* Route = RouteWeak.Get())
			Route->GenerateRandomRoute();
	}

	// 모든 레벨 초기화 완료 후 레벨 매니저 스폰
	FActorSpawnParameters SpawnParameter          = FActorSpawnParameters();
	SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	LevelManager = GetWorld()->SpawnActor<AOmnibusLevelManager>(AOmnibusLevelManager::StaticClass(), SpawnParameter);
	LevelManager->Initialize(this);


	FOmniAsync::OnPostDataUpdate.AddUObject(this, &AOmnibusRoadManager::NotifyUpdateBusRoute);
}

void AOmnibusRoadManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOmnibusRoadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FillPassengerPool();
}

void AOmnibusRoadManager::NotifyUpdateBusRoute()
{
	// 버스 노선 변경을 하차 승객에게 간접적으로 전파.
	++BusRouteMapVersion;

	// 환승 경로 없는 passenger 재시도
	for (const TWeakObjectPtr<AOmniPassenger>& NoPathPassengerWeak : NoTransferPathPassengerList)
	{
		if (AOmniPassenger* NoPathPassenger = NoPathPassengerWeak.Get())
			NoPathPassenger->RetryJourneyFromSector();
	}
	NoTransferPathPassengerList.Reset();

	// 정류장에 있는 승객 재시도
	for (const auto& [Key, PassengerWeak] : OmniPassengerTMap)
	{
		if (AOmniPassenger* Passenger = PassengerWeak.Get())
			Passenger->RetryJourneyFromStop();
	}
}

void AOmnibusRoadManager::AddNoTransferPathPassenger(AOmniPassenger* InPassenger)
{
	NoTransferPathPassengerList.Emplace(InPassenger);
}

std::tuple<FSectorInfo, TArray<FSectorInfo>> AOmnibusRoadManager::GetHomeAndDests()
{
	FSectorInfo HomeInfo;
	TArray<FSectorInfo> DestInfoList;
	UT_IF(OmniCityBlockList.Num() <= 2)
		return std::make_tuple(HomeInfo, DestInfoList);

	// 출발지
	const int32 HomeBlockIdx        = UtlMath::GetIntRandom(0, OmniCityBlockList.Num() - 1);
	AOmniCityBlock* const HomeBlock = OmniCityBlockList[HomeBlockIdx].Get();
	const int32 HomeSectorIdx       = UtlMath::GetIntRandom(0, HomeBlock->GetSectorNum() - 1);

	constexpr int LoopMax = 1000;

	// 도착지
	int32 DestBlockIdx = UtlMath::GetIntRandom(0, OmniCityBlockList.Num() - 1);
	for (int LoopCount = 0; LoopCount < LoopMax; ++LoopCount)
	{
		if (HomeBlockIdx != DestBlockIdx)
			break;
		DestBlockIdx = UtlMath::GetIntRandom(0, OmniCityBlockList.Num() - 1);
	}

	AOmniCityBlock* DestBlock = OmniCityBlockList[DestBlockIdx].Get();
	int32 DestSectorIdx       = UtlMath::GetIntRandom(0, DestBlock->GetSectorNum() - 1);
	
	// 이웃 블록인 경우, 섹터가 이웃하지 않게 조정.
	if (HomeBlock->IsNeighborBlock(DestBlock))
	{
		bool bNeighborSector = true;
		for (int LoopCount = 0; LoopCount < LoopMax; ++LoopCount)
		{
			if (HomeBlock->IsNeighborSector(HomeSectorIdx, DestBlock, DestSectorIdx) == false)
			{
				bNeighborSector = false;
				break;
			}
			DestBlockIdx = UtlMath::GetIntRandom(0, OmniCityBlockList.Num() - 1);
		}

		// 이웃하지 않은 섹터를 찾지 못한 경우, 이웃하지 않은 블록과 섹터 선택
		if (bNeighborSector)
		{
			for (int LoopCount = 0; LoopCount < LoopMax; ++LoopCount)
			{
				DestBlock = OmniCityBlockList[DestBlockIdx].Get();
				if (HomeBlock->IsNeighborBlock(DestBlock) == false)
					break;
				DestBlockIdx = UtlMath::GetIntRandom(0, OmniCityBlockList.Num() - 1);
			}
			DestSectorIdx = UtlMath::GetIntRandom(0, DestBlock->GetSectorNum() - 1);
		}
	}

	HomeInfo     = FSectorInfo(HomeBlock, HomeSectorIdx);
	DestInfoList = {FSectorInfo(DestBlock, DestSectorIdx)};

	return std::make_tuple(HomeInfo, DestInfoList);
}

void AOmnibusRoadManager::FillPassengerPool()
{
	if (bPoolInsufficient == false)
		return;

	// 부하를 줄이기 위해 틱당 스폰되는 Passenger 수를 조절.
	constexpr int SpawnPerTick = 2;
	for (int Count = 0; Count < SpawnPerTick; ++Count)
	{
		GeneratePassengerInPool();
	}

	if (PassengerPool->IsFull())
	{
		SetActorTickEnabled(false);
		bPoolInsufficient = false;
	}
}

void AOmnibusRoadManager::GeneratePassengerInPool()
{
	if (PassengerPool->IsFull())
		return;

	const UOmnibusPlayData* PlayData = GetOmnibusPlayData();
	if (PlayData == nullptr)
		return;

	const FTransform SpawnTransform = FTransform(FVector(100'000, 100'000, -100'000));
	AOmniPassenger* NewPassenger = GetWorld()->SpawnActorDeferred<AOmniPassenger>(PlayData->GetPassengerClass()
	                                                                            , SpawnTransform, nullptr, nullptr
	                                                                            , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	NewPassenger->DisablePassenger();
	PassengerPool->Enqueue(NewPassenger);

	NewPassenger->FinishSpawning(SpawnTransform);
}

AOmniPassenger* AOmnibusRoadManager::PopPassengerInPool()
{
	if (PassengerPool->IsEmpty())
	{
		GeneratePassengerInPool();
	}

	TWeakObjectPtr<AOmniPassenger> FrontPassengerWeak;
	PassengerPool->Dequeue(FrontPassengerWeak);
	AOmniPassenger* Passenger = FrontPassengerWeak.Get();
	UT_IF(Passenger == nullptr)
		return nullptr;

	Passenger->EnablePassenger();
	AddPassenger(Passenger);

	if (PassengerPool->Count() < PoolLowerThreshold)
	{
		SetActorTickEnabled(true);
		bPoolInsufficient = true;
	}

	return Passenger;
}


void AOmnibusRoadManager::SetRoutesRender(const bool InVisibility)
{
	bRouteVisible = InVisibility;
	for (const auto& [Key, RouteWeak] : OmniRouteTMap)
	{
		if (AOmniLineBusRoute* Route = RouteWeak.Get())
			Route->SetRouteRender(InVisibility);
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

void AOmnibusRoadManager::SetPassengerRender(const bool InVisibility)
{
	bPassengerVisible = InVisibility;
	for (const auto& [Key, PassengerWeak] : OmniPassengerTMap)
	{
		if (AOmniPassenger* Passenger = PassengerWeak.Get())
			Passenger->SetMeshVisibility(bPassengerVisible);
	}
}

APathVisualizator* AOmnibusRoadManager::VisualizePath(const FTransferPath& InTransferPath)
{
	// 캐시에서 찾기.
	// 찾으면서 겸사겸사 SetPathVisibility를 off하기 위해 for문 사용.
	int FindVisualIdx = INDEX_NONE;
	for (int idx = 0; idx < PathVisualizatorCacheList.Num(); ++idx)
	{
		const APathVisualizator* VisualPtr = PathVisualizatorCacheList[idx].Get();
		if (VisualPtr == nullptr)
			continue;

		if (VisualPtr->IsSamePath(InTransferPath))
		{
			FindVisualIdx = idx;
		}
		else
		{ 
			VisualPtr->SetPathVisibility(false);
		}
	}
	if (FindVisualIdx != INDEX_NONE)
	{
		TWeakObjectPtr<APathVisualizator> VisualWeak = PathVisualizatorCacheList[FindVisualIdx];
		PathVisualizatorCacheList.RemoveAt(FindVisualIdx, 1, false);
		PathVisualizatorCacheList.Emplace(VisualWeak);
	}
	else if (PathVisualizatorCacheList.Num() < VisualizatorCacheSize)
	{
		FActorSpawnParameters SpawnParameter          = FActorSpawnParameters();
		SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APathVisualizator* Visualizator = GetWorld()->SpawnActor<APathVisualizator>(GetOmnibusPlayData()->GetPathVisualizatorClass(), GetTransform(), SpawnParameter);
		PathVisualizatorCacheList.Emplace(Visualizator);
	}
	else
	{
		TWeakObjectPtr<APathVisualizator> VisualWeak = PathVisualizatorCacheList[0];
		PathVisualizatorCacheList.RemoveAt(0, 1, false);
		PathVisualizatorCacheList.Emplace(VisualWeak);
	}

	APathVisualizator* LastVisual = PathVisualizatorCacheList.Last().Get();
	LastVisual->VisualizeTransferRoutes(InTransferPath);
	LastVisual->SetPathVisibility(true);

	return LastVisual;
}


//~=============================================================================
// 시스템 관리 함수.

void AOmnibusRoadManager::ExecuteAutoManageFunction()
{
	CollectOmniRoad();
	ExecuteAllRoadsDetect();
	CleanupInvalidStationInRoad();

	CollectBusStop();
	ExecuteAllStationsDetect();

	CollectCityBlock();
	CleanupInvalidStopInCityBlock();

	CollectRoute();

	// CollectPassenger();
}

void AOmnibusRoadManager::CollectOmniRoad()
{
	OmniRoadsTMap.Reset();
	TArray<AActor*> RoadList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniRoad::StaticClass(), RoadList);

	for (AActor* RoadListActor : RoadList)
	{
		if (AOmniRoad* OmniRoad = Cast<AOmniRoad>(RoadListActor))
			AddOmniRoad(OmniRoad);
	}
}

void AOmnibusRoadManager::ExecuteAllRoadsDetect()
{
	for (const auto& [Key, RoadWeak] : OmniRoadsTMap)
	{
		if (AOmniRoad* Road = RoadWeak.Get())
			Road->DetectAllConnectedOmniRoad();
	}
}

void AOmnibusRoadManager::CleanupInvalidStationInRoad()
{
	for (const auto& [Key, RoadWeak] : OmniRoadsTMap)
	{
		if (AOmniRoad* Road = RoadWeak.Get())
			Road->RemoveInvalidBusStop();
	}
}

void AOmnibusRoadManager::CollectBusStop()
{
	OmniStationTMap.Reset();
	TArray<AActor*> StopList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniStationBusStop::StaticClass(), StopList);

	for (AActor* SpotActor : StopList)
	{
		if (AOmniStationBusStop* Stop = Cast<AOmniStationBusStop>(SpotActor))
			AddStation(Stop);
	}
}

void AOmnibusRoadManager::ExecuteAllStationsDetect()
{
	for (const auto& [Key, StationWeak] : OmniStationTMap)
	{
		if (AOmniStationBusStop* Station = StationWeak.Get())
			Station->SearchRoadAndBlock();
	}
}

void AOmnibusRoadManager::CollectCityBlock()
{
	OmniCityBlockList.Reset();
	TArray<AActor*> CityBlockList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniCityBlock::StaticClass(), CityBlockList);

	for (AActor* BlockActor : CityBlockList)
	{
		if (AOmniCityBlock* Block = Cast<AOmniCityBlock>(BlockActor))
			AddCityBlock(Block);
	}
}

void AOmnibusRoadManager::CleanupInvalidStopInCityBlock()
{
	for (const TWeakObjectPtr<AOmniCityBlock>& CityBlockWeak : OmniCityBlockList)
	{
		if (AOmniCityBlock* CityBlock = CityBlockWeak.Get())
			CityBlock->RemoveInvalidBusStop();
	}
}

void AOmnibusRoadManager::CollectRoute()
{
	OmniRouteTMap.Reset();
	TArray<AActor*> BusLineList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniLineBusRoute::StaticClass(), BusLineList);

	for (AActor* BusLineActor : BusLineList)
	{
		if (AOmniLineBusRoute* BusLine = Cast<AOmniLineBusRoute>(BusLineActor))
			AddOmniRoute(BusLine);
	}
}

void AOmnibusRoadManager::CollectPassenger()
{
	OmniPassengerTMap.Reset();
	TArray<AActor*> PassengerList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniPassenger::StaticClass(), PassengerList);

	for (AActor* PassengerActor : PassengerList)
	{
		if (AOmniPassenger* Passenger = Cast<AOmniPassenger>(PassengerActor))
			AddPassenger(Passenger);
	}
}

void AOmnibusRoadManager::AddOmniRoad(AOmniRoad* InRoad)
{
	if (UT_IS_VALID(InRoad) == false)
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
	if (UT_IS_VALID(InRoad) == false)
		return;

	RemoveOmniRoadByID(InRoad->GetOmniID());
}

AOmniRoad* AOmnibusRoadManager::FindOmniRoad(const uint64 InId)
{
	const TWeakObjectPtr<AOmniRoad>* FindPtr = OmniRoadsTMap.Find(InId);

	return (FindPtr != nullptr) ? FindPtr->Get() : nullptr;
}

void AOmnibusRoadManager::AddStation(AOmniStationBusStop* InStation)
{
	if (UT_IS_VALID(InStation) == false)
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
	if (UT_IS_VALID(InStation) == false)
		return;

	RemoveStationByID(InStation->GetOmniID());
}

AOmniStationBusStop* AOmnibusRoadManager::FindStation(const uint64 InId)
{
	const TWeakObjectPtr<AOmniStationBusStop>* FindPtr = OmniStationTMap.Find(InId);

	return (FindPtr != nullptr) ? FindPtr->Get() : nullptr;
}

void AOmnibusRoadManager::AddCityBlock(AOmniCityBlock* InCityBlock)
{
	if (UT_IS_VALID(InCityBlock) == false)
		return;

	OmniCityBlockList.Emplace(InCityBlock);
}

void AOmnibusRoadManager::RemoveCityBlockByID(const uint64 InId)
{
	UtlContainer::RemoveSwapByPredicate(OmniCityBlockList, [InId](const TWeakObjectPtr<AOmniCityBlock>& InCityBlockWeak)
	{
		AOmniCityBlock* CityBlock = InCityBlockWeak.Get();
		return CityBlock ? CityBlock->GetOmniID() == InId : false;
	});
}

void AOmnibusRoadManager::RemoveCityBlock(AOmniCityBlock* InCityBlock)
{
	if (UT_IS_VALID(InCityBlock) == false)
		return;

	RemoveCityBlockByID(InCityBlock->GetOmniID());
}

AOmniCityBlock* AOmnibusRoadManager::FindCityBlock(const uint64 InId)
{
	const TWeakObjectPtr<AOmniCityBlock>* FindPtr = OmniCityBlockList.FindByPredicate([InId](const TWeakObjectPtr<AOmniCityBlock>& InCityBlockWeak)
	{
		AOmniCityBlock* CityBlock = InCityBlockWeak.Get();
		return CityBlock ? CityBlock->GetOmniID() == InId : false;
	});

	return (FindPtr != nullptr) ? FindPtr->Get() : nullptr;
}

void AOmnibusRoadManager::AddOmniRoute(AOmniLineBusRoute* InOmniRoute)
{
	if (UT_IS_VALID(InOmniRoute) == false)
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
	if (UT_IS_VALID(InOmniRoute) == false)
		return;

	RemoveOmniRouteByID(InOmniRoute->GetOmniID());
}

AOmniLineBusRoute* AOmnibusRoadManager::FindOmniRoute(const uint64 InId)
{
	const TWeakObjectPtr<AOmniLineBusRoute>* FindPtr = OmniRouteTMap.Find(InId);

	return (FindPtr != nullptr) ? FindPtr->Get() : nullptr;
}

void AOmnibusRoadManager::AddPassenger(AOmniPassenger* InPassenger)
{
	if (UT_IS_VALID(InPassenger) == false)
		return;

	TWeakObjectPtr<AOmniPassenger>& PassengerMapValue = OmniPassengerTMap.FindOrAdd(InPassenger->GetOmniID());
	PassengerMapValue = InPassenger;

	UOmniPlayMainUI::OnPostPassengersNum.Broadcast(1);
}

void AOmnibusRoadManager::RemovePassengerByID(const uint64 InId)
{
	OmniPassengerTMap.Remove(InId);
}

void AOmnibusRoadManager::RemovePassenger(AOmniLineBusRoute* InPassenger)
{
	if (UT_IS_VALID(InPassenger) == false)
		return;

	RemovePassengerByID(InPassenger->GetOmniID());
}

AOmniPassenger* AOmnibusRoadManager::FindPassenger(const uint64 InId)
{
	const TWeakObjectPtr<AOmniPassenger>* FindPtr = OmniPassengerTMap.Find(InId);

	return (FindPtr != nullptr) ? FindPtr->Get() : nullptr;
}
