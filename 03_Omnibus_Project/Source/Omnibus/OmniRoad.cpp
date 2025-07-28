// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniRoad.h"

#include "OmniConnectorComponent.h"
#include "OmniStationBusStop.h"
#include "Components/SplineComponent.h"

#include "UtlLog.h"
#include "UTLStatics.h"

AOmniRoad::AOmniRoad()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OmniRoadRoot"));
	RootComponent->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultPlacedMesh(TEXT("/Game/Meshs/RoadVertex.RoadVertex"));
	if (DefaultPlacedMesh.Succeeded())
		PlacedMesh = DefaultPlacedMesh.Object;

	InitArrays(0, 0, 0);
}

void AOmniRoad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DetectAllConnectedOmniRoad();
}

#if WITH_EDITOR
void AOmniRoad::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	SetZ_Axis();
	DetectAllConnectedOmniRoad();
}
#endif // WITH_EDITOR

void AOmniRoad::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniRoad::Destroyed()
{
	RemoveAllConnectedRoadsBoth();
	Super::Destroyed();
}

void AOmniRoad::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllConnectedRoadsBoth();

	Super::EndPlay(EndPlayReason);
}

void AOmniRoad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniRoad::DetectAllConnectedOmniRoad()
{
	// 현재 도로의 감지용 컴포넌트 순회. 감지된 대상 도로를 찾음.
	for (UOmniConnectorComponent* Connector : RoadConnectors)
	{
		if (UT_IS_VALID(Connector) == false)
			continue;

		DetectOmniRoad(Connector);
	}
}

void AOmniRoad::InitArrays(const uint8 InRoadSplineNum, const uint8 InRoadConnectorNum, const uint8 InLaneSplineNum)
{
	RoadSplineNum    = InRoadSplineNum;
	RoadConnectorNum = InRoadConnectorNum;
	LaneSplineNum    = InLaneSplineNum;

	ConnectedRoadsArray.Reset();
	RoadConnectors.Reset();
	RoadSplines.Reset();
	LaneSplines.Reset();

	ConnectedRoadsArray.SetNum(RoadConnectorNum);
	RoadConnectors.SetNum(RoadConnectorNum);
	RoadSplines.SetNum(RoadSplineNum);
	LaneSplines.SetNum(LaneSplineNum);
}

void AOmniRoad::InitRoadSpline(const int32 InIdx)
{
	check(RoadSplines.IsValidIndex(InIdx))

	const FName RoadSplineName            = UtlStr::ConcatStrInt(TEXT("RoadSpline"), InIdx);
	USplineComponent* const CurRoadSpline = CreateDefaultSubobject<USplineComponent>(RoadSplineName);

	if (UT_IS_VALID(CurRoadSpline) == false)
		return;

	RoadSplines[InIdx] = CurRoadSpline;
	CurRoadSpline->SetupAttachment(RootComponent);
	CurRoadSpline->SetMobility(EComponentMobility::Static);
}

void AOmniRoad::InitRoadConnector(const int32 InIdx, USplineComponent* InOwnerRoadSpline, const int32 InSplinePointIdx)
{
	check(RoadConnectors.IsValidIndex(InIdx))
	if (UT_IS_VALID(InOwnerRoadSpline) == false)
		return;

	const FName ConnectorName = UtlStr::ConcatStrInt(TEXT("RoadConnector"), InIdx);
	UOmniConnectorComponent* const CurConnector = CreateDefaultSubobject<UOmniConnectorComponent>(ConnectorName);

	if (UT_IS_VALID(CurConnector) == false)
		return;

	RoadConnectors[InIdx] = CurConnector;
	CurConnector->SetupAttachment(InOwnerRoadSpline);
	CurConnector->SetOwnerOmniRoadAndSpline(this, InOwnerRoadSpline, InSplinePointIdx, InIdx);
	CurConnector->SetCollisionProfileName(EOmniCollisionProfile::RoadConnectorOverlap);
	// CurConnector->SetMobility(EComponentMobility::Static);
}

void AOmniRoad::InitLaneSpline(const int32 InIdx, USplineComponent* InOwnerRoadSpline)
{
	check(LaneSplines.IsValidIndex(InIdx))
	if (UT_IS_VALID(InOwnerRoadSpline) == false)
		return;

	const FName LaneSplineName            = UtlStr::ConcatStrInt(TEXT("LaneSpline"), InIdx);
	USplineComponent* const CurLaneSpline = CreateDefaultSubobject<USplineComponent>(LaneSplineName);
	if (UT_IS_VALID(CurLaneSpline) == false)
		return;

	LaneSplines[InIdx] = CurLaneSpline;
	CurLaneSpline->SetupAttachment(InOwnerRoadSpline);
	CurLaneSpline->SetMobility(EComponentMobility::Static);
}

USplineComponent* AOmniRoad::GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	const int32 LaneIdx = FindLaneIdxToNextRoad(InPrevRoad, InNextTargetRoad);
	
	return LaneIdx == INDEX_NONE ? nullptr : GetLaneSpline(LaneIdx);
}

AOmniRoad* AOmniRoad::GetNextRoadByLaneIdx(const int32 InLaneIdx)
{
	UT_LOG("omniRoad에서 로드됨")

	return nullptr;
}

int32 AOmniRoad::FindLaneIdxToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	UT_LOG("omniRoad에서 로드됨")

	return INDEX_NONE;
}

std::tuple<USplineComponent*, TArray<FBusStopDistance>> AOmniRoad::GetLaneAndBusStop(const int32 InLaneIdx) const
{
	USplineComponent* ResPathLaneSpline = nullptr;
	TArray<FBusStopDistance> ResBusStopsOnPath;

	if (InLaneIdx == INDEX_NONE || LaneSplines.IsValidIndex(InLaneIdx) == false)
		return std::make_tuple(ResPathLaneSpline, ResBusStopsOnPath);

	ResPathLaneSpline = GetLaneSpline(InLaneIdx);

	// 해당 차선 위에 있는 버스 정류장 추적 및 배치순 정렬.
	for (const TWeakObjectPtr<AOmniStationBusStop>& BusStopWeak : OwnedBusStops)
	{
		const AOmniStationBusStop* BusStop = BusStopWeak.Get();
		if (BusStop == nullptr)
			continue;

		if (BusStop->GetOwnerLaneIdx() == InLaneIdx)
		{
			const float StopInputKey       = BusStop->GetClosestLaneInputKey();
			const float DistanceByInputKey = GetLaneSpline(InLaneIdx)->GetDistanceAlongSplineAtSplineInputKey(StopInputKey);
			ResBusStopsOnPath.Emplace(BusStopWeak, DistanceByInputKey);
		}
	}
	ResBusStopsOnPath.Sort([](const FBusStopDistance& A, const FBusStopDistance& B)
	{
		return A.Distance < B.Distance;
	});

	return std::make_tuple(ResPathLaneSpline, ResBusStopsOnPath);
}

USplineComponent* AOmniRoad::GetRoadSpline(const int32 InIdx) const
{
	check(RoadSplines.IsValidIndex(InIdx))

	if (IsValid(RoadSplines[InIdx]))
		return RoadSplines[InIdx];

	return nullptr;
}

USplineComponent* AOmniRoad::GetLaneSpline(const int32 InIdx) const
{
	check(LaneSplines.IsValidIndex(InIdx))

	if (IsValid(LaneSplines[InIdx]))
		return LaneSplines[InIdx];

	return nullptr;
}

void AOmniRoad::AddConnectedRoadSingle(AOmniRoad* InRoad, const int32 InAccessIdx)
{
	if (UT_IS_VALID(InRoad) && (InRoad != this))
	{
		if (ConnectedRoadsArray.IsValidIndex(InAccessIdx))
			ConnectedRoadsArray[InAccessIdx] = InRoad;
		else
			UT_LOG("InAccessIdx : invalid value")
	}
}

void AOmniRoad::AddConnectedRoadBoth(const UOmniConnectorComponent* InTargetConnector, const int32 InMyAccessIdx)
{
	AOmniRoad* const TargetRoad = InTargetConnector->GetOwnerOmniRoad();
	const int32 TargetAccessIdx = InTargetConnector->GetAccessPointIdx();
	if (UT_IS_VALID(TargetRoad))
	{
		AddConnectedRoadSingle(TargetRoad, InMyAccessIdx);
		TargetRoad->AddConnectedRoadSingle(this, TargetAccessIdx);
	}
}

void AOmniRoad::RemoveConnectedRoadSingle(const int32 InAccessIdx)
{
	if (ConnectedRoadsArray.IsValidIndex(InAccessIdx))
		ConnectedRoadsArray[InAccessIdx] = nullptr;
}

void AOmniRoad::RemoveConnectedRoadSingle(AOmniRoad* InRoad)
{
	if (IsValid(InRoad) == false)
		return;

	const int32 FindIdx = FindConnectedRoadIdx(InRoad);
	if (FindIdx != INDEX_NONE)
		RemoveConnectedRoadSingle(FindIdx);
}

void AOmniRoad::RemoveConnectedRoadBoth(const int32 InAccessIdx)
{
	if(ConnectedRoadsArray.IsValidIndex(InAccessIdx) == false)
	{
		UT_LOG("InAccessIdx 범주 초과")
		return;
	}

	AOmniRoad* ConnectedRoad = ConnectedRoadsArray[InAccessIdx];
	if (IsValid(ConnectedRoad))
		ConnectedRoad->RemoveConnectedRoadSingle(this);

	RemoveConnectedRoadSingle(InAccessIdx);
}

void AOmniRoad::RemoveConnectedRoadBoth(AOmniRoad* InRoad)
{
	if (IsValid(InRoad))
	{
		RemoveConnectedRoadSingle(InRoad);
		InRoad->RemoveConnectedRoadSingle(this);
	}
}

void AOmniRoad::RemoveAllConnectedRoadsBoth()
{
	for (AOmniRoad* ConnectedRoad : ConnectedRoadsArray)
	{
		if (IsValid(ConnectedRoad))
			ConnectedRoad->RemoveConnectedRoadSingle(this);
	}
	ConnectedRoadsArray.Reset();
}

AOmniStationBusStop* AOmniRoad::FindBusStopByID(const uint64 InId)
{
	const TWeakObjectPtr<AOmniStationBusStop>* FindPtr = OwnedBusStops.FindByPredicate([InId](const TWeakObjectPtr<AOmniStationBusStop>& InBusStop) -> bool
	{
		if (InBusStop.IsValid())
			return (InBusStop.Get()->GetOmniID() == InId);
		return false;
	});
	
	if (FindPtr == nullptr)
		return nullptr;

	return FindPtr->Get();
}

int32 AOmniRoad::FindBusStopIdx(AOmniStationBusStop* InBusStop) const
{
	if(IsValid(InBusStop) == false)
		return INDEX_NONE;
	return OwnedBusStops.Find(InBusStop);
}

bool AOmniRoad::HasBusStopByID(const uint64 InId)
{
	return (FindBusStopByID(InId) != nullptr);
}

bool AOmniRoad::HasBusStop(AOmniStationBusStop* InBusStop) const
{
	return FindBusStopIdx(InBusStop) != INDEX_NONE;
}

void AOmniRoad::AddBusStop(AOmniStationBusStop* InBusStop)
{
	if (UT_IS_VALID(InBusStop))
	{
		if (HasBusStop(InBusStop) == false)
			OwnedBusStops.Emplace(InBusStop);
	}
}

void AOmniRoad::RemoveBusStopByID(const uint64 InId)
{
	AOmniStationBusStop* FindBusStop = FindBusStopByID(InId);
	RemoveBusStop(FindBusStop);
}

void AOmniRoad::RemoveBusStop(AOmniStationBusStop* InBusStop)
{
	if (IsValid(InBusStop))
	{
		// 해당 버스 정류장 삭제할 때, 유효하지 않은 배열 요소도 같이 제거.
		OwnedBusStops.RemoveAll([&InBusStop](TWeakObjectPtr<AOmniStationBusStop>& Element)
		{
			return (Element == InBusStop) || (Element.IsValid() == false);
		});
	}
}

void AOmniRoad::RemoveInvalidBusStop()
{
	// 유효하지 않거나, 연결되지 않은 버스 정류장 제거
	OwnedBusStops.RemoveAll([this](TWeakObjectPtr<AOmniStationBusStop>& Element)
	{
		if (Element.IsValid() == false)
			return true;
		if (Element.Get()->GetOwnerOmniRoad() != this)
			return true;
		return false;
	});
}

TArray<TWeakObjectPtr<AOmniStationBusStop>> AOmniRoad::GetOwnedBusStops()
{
	return OwnedBusStops;
}

int32 AOmniRoad::FindConnectedRoadIdx(AOmniRoad* InRoad) const
{
	if (IsValid(InRoad) == false)
		return INDEX_NONE;

	return ConnectedRoadsArray.Find(InRoad);
}

AOmniRoad* AOmniRoad::GetRandomNextRoad(AOmniRoad* InPrevRoad)
{
	const int32 PrevRoadIdx = FindConnectedRoadIdx(InPrevRoad);
	
	constexpr int LoopLimit = 1000;
	for (int i = 0; i < LoopLimit; ++i)
	{
		const int32 RdIdx = UtlMath::GetIntRandom(0, ConnectedRoadsArray.Num() - 1);
		if ((RdIdx != PrevRoadIdx) && ConnectedRoadsArray.IsValidIndex(RdIdx) && IsValid(ConnectedRoadsArray[RdIdx]))
			return ConnectedRoadsArray[RdIdx];
	}
	return nullptr;
}

AOmniRoad* AOmniRoad::GetConnectedRoad(const int32 ConnectedRoadIdx) const
{
	check(ConnectedRoadsArray.IsValidIndex(ConnectedRoadIdx))

	if (IsValid(ConnectedRoadsArray[ConnectedRoadIdx]))
		return ConnectedRoadsArray[ConnectedRoadIdx];

	return nullptr;
}

double AOmniRoad::GetRoadWidth()
{
	if (UT_IS_VALID(PlacedMesh) == false)
		return 0.0;

	const FBox PlaceMeshBox = PlacedMesh->GetBoundingBox();
	return UtlMath::GetBoxWidth(PlaceMeshBox);
}

UOmniConnectorComponent* AOmniRoad::DetectOmniRoad(UOmniConnectorComponent* Connector)
{
	UClass* ConnectorClass = Connector->StaticClass();

	//겹치는 컴포넌트 추적.
	TArray<UPrimitiveComponent*> OverlappingComps;
	FUtlStatics::GetOverlapComps(Connector, ConnectorClass, OverlappingComps);
	UPrimitiveComponent** FindPtr = OverlappingComps.FindByPredicate([ConnectorClass](const UPrimitiveComponent* InOverlap) -> bool
	{
		return InOverlap->IsA(ConnectorClass);
	});

	// 겹치는게 없다면, 연결된 도로 제거. 있다면 추가함.
	if (FindPtr == nullptr)
	{
		const int32 AccessPointIdx = Connector->GetAccessPointIdx();
		RemoveConnectedRoadBoth(AccessPointIdx);
		return nullptr;
	}

	UOmniConnectorComponent* DetectedTargetConnector = Cast<UOmniConnectorComponent>(*FindPtr);
	AddConnectedRoadBoth(DetectedTargetConnector, Connector->GetAccessPointIdx());
	return DetectedTargetConnector;
}

bool AOmniRoad::CanInstallBusStop() const
{
	UT_LOG("omniRoad에서 로드됨")

	return false;
}

bool AOmniRoad::FindNearestRoadAndLane(const TArray<AActor*>& InActors, const FVector& InTargetPos, AOmniRoad*& OutNearRoad, int32& OutLaneIdx)
{
	bool bIsFind = false;

	double DistanceFromNearest = TNumericLimits<double>::Max();
	for (AActor* InActor : InActors)
	{
		if (AOmniRoad* Road = Cast<AOmniRoad>(InActor))
		{
			const uint8 LaneNum = Road->GetLaneSplineNum();
			for (int idx = 0; idx < LaneNum; ++idx)
			{
				const USplineComponent* Lane  = Road->GetLaneSpline(idx);
				FVector ClosestSplineLoc      = Lane->FindLocationClosestToWorldLocation(InTargetPos, ESplineCoordinateSpace::World);
				const double DistanceFromLane = (InTargetPos - ClosestSplineLoc).SizeSquared();
				if (DistanceFromLane < DistanceFromNearest)
				{
					DistanceFromNearest = DistanceFromLane;

					OutNearRoad = Road;
					OutLaneIdx  = idx;
					bIsFind     = true;
				}
			}
		}
	}

	return bIsFind;
}
