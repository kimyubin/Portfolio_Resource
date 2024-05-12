// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniRoad.h"

#include <random>

#include "Omnibus.h"
#include "OmnibusUtilities.h"
#include "OmniConnectorComponent.h"
#include "OmniStationBusStop.h"
#include "Components/SplineComponent.h"

AOmniRoad::AOmniRoad()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OmniRoadRoot"));

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
	for (UOmniConnectorComponent* const Connector : RoadConnectors)
	{
		if (OB_IS_VALID(Connector) == false)
			continue;

		DetectOmniRoad(Connector);
	}
}

void AOmniRoad::InitArrays(const uint8 InRoadSplineNum, const uint8 InRoadConnectorNum, const uint8 InLaneSplineNum)
{
	RoadSplineNum    = InRoadSplineNum;
	RoadConnectorNum = InRoadConnectorNum;
	LaneSplineNum    = InLaneSplineNum;

	ConnectedRoadsArray.Empty();
	RoadConnectors.Empty();
	RoadSplines.Empty();
	LaneSplines.Empty();

	ConnectedRoadsArray.SetNum(RoadConnectorNum);
	RoadConnectors.SetNum(RoadConnectorNum);
	RoadSplines.SetNum(RoadSplineNum);
	LaneSplines.SetNum(LaneSplineNum);
}

void AOmniRoad::InitRoadSpline(const uint32 InIdx)
{
	check(RoadSplines.IsValidIndex(InIdx))

	const FName RoadSplineName            = OmniStr::ConcatStrInt("RoadSpline", InIdx);
	USplineComponent* const CurRoadSpline = CreateDefaultSubobject<USplineComponent>(RoadSplineName);

	if (OB_IS_VALID(CurRoadSpline) == false)
		return;

	RoadSplines[InIdx] = CurRoadSpline;
	CurRoadSpline->SetupAttachment(RootComponent);
}

void AOmniRoad::InitRoadConnector(const uint32 InIdx, USplineComponent* InOwnerRoadSpline, const uint32 InSplinePointIdx)
{
	check(RoadConnectors.IsValidIndex(InIdx))
	if (OB_IS_VALID(InOwnerRoadSpline) == false)
		return;

	const FName ConnectorName = OmniStr::ConcatStrInt("RoadConnector", InIdx);
	UOmniConnectorComponent* const CurConnector = CreateDefaultSubobject<UOmniConnectorComponent>(ConnectorName);

	if (OB_IS_VALID(CurConnector) == false)
		return;

	RoadConnectors[InIdx] = CurConnector;
	CurConnector->SetupAttachment(InOwnerRoadSpline);
	CurConnector->SetOwnerOmniRoadAndSpline(this, InOwnerRoadSpline, InSplinePointIdx, InIdx);
	CurConnector->SetCollisionProfileName(EOmniCollisionProfile::RoadConnector);
}

void AOmniRoad::InitLaneSpline(const uint32 InIdx, USplineComponent* InOwnerRoadSpline)
{
	check(LaneSplines.IsValidIndex(InIdx))
	if (OB_IS_VALID(InOwnerRoadSpline) == false)
		return;

	const FName LaneSplineName            = OmniStr::ConcatStrInt("LaneSpline", InIdx);
	USplineComponent* const CurLaneSpline = CreateDefaultSubobject<USplineComponent>(LaneSplineName);
	if (OB_IS_VALID(CurLaneSpline) == false)
		return;

	LaneSplines[InIdx] = CurLaneSpline;
	CurLaneSpline->SetupAttachment(InOwnerRoadSpline);
}

USplineComponent* AOmniRoad::GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	OB_LOG("omniRoad에서 로드됨")

	return nullptr;
}


USplineComponent* AOmniRoad::GetRoadSpline(const uint32 InIdx) const
{
	check(RoadSplines.IsValidIndex(InIdx))

	if (IsValid(RoadSplines[InIdx]))
		return RoadSplines[InIdx];

	return nullptr;
}

USplineComponent* AOmniRoad::GetLaneSpline(const uint32 InIdx) const
{
	check(LaneSplines.IsValidIndex(InIdx))

	if (IsValid(LaneSplines[InIdx]))
		return LaneSplines[InIdx];

	return nullptr;
}

void AOmniRoad::AddConnectedRoadSingle(AOmniRoad* InRoad, const uint8 InAccessIdx)
{
	if (OB_IS_VALID(InRoad) && (InRoad != this))
	{
		if (ConnectedRoadsArray.IsValidIndex(InAccessIdx))
			ConnectedRoadsArray[InAccessIdx] = InRoad;
		else
			OB_LOG("InAccessIdx : invalid value")
	}
}

void AOmniRoad::AddConnectedRoadBoth(const UOmniConnectorComponent* InTargetConnector, const uint8 InMyAccessIdx)
{
	AOmniRoad* const TargetRoad = InTargetConnector->GetOwnerOmniRoad();
	const uint8 TargetAccessIdx = InTargetConnector->GetAccessPointIdx();
	if (OB_IS_VALID(TargetRoad))
	{
		AddConnectedRoadSingle(TargetRoad, InMyAccessIdx);
		TargetRoad->AddConnectedRoadSingle(this, TargetAccessIdx);
	}
}

void AOmniRoad::RemoveConnectedRoadSingle(const uint8 InAccessIdx)
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

void AOmniRoad::RemoveConnectedRoadBoth(const uint8 InAccessIdx)
{
	if(ConnectedRoadsArray.IsValidIndex(InAccessIdx) == false)
	{
		OB_LOG("InAccessIdx 범주 초과")
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
	ConnectedRoadsArray.Empty();
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
	if (OB_IS_VALID(InBusStop))
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

int32 AOmniRoad::FindConnectedRoadIdx(AOmniRoad* InRoad) const
{
	if (IsValid(InRoad) == false)
		return INDEX_NONE;

	return ConnectedRoadsArray.Find(InRoad);
}

void AOmniRoad::FindPath(AOmniRoad* InStartRoad, AOmniRoad* InEndRoad, AOmniRoad* PrevRoad, TArray<AOmniRoad*>& OutPath)
{
}

AOmniRoad* AOmniRoad::GetRandomNextRoad(AOmniRoad* InPrevRoad)
{
	const int32 PrevRoadIdx = FindConnectedRoadIdx(InPrevRoad);
	
	constexpr int LoopLimit = 1000;
	for (int i = 0; i < LoopLimit; ++i)
	{
		const int32 RdIdx = OmniMath::GetIntRandom(0, ConnectedRoadsArray.Num() - 1);
		if ((RdIdx != PrevRoadIdx) && ConnectedRoadsArray.IsValidIndex(RdIdx) && IsValid(ConnectedRoadsArray[RdIdx]))
			return ConnectedRoadsArray[RdIdx];
	}
	return nullptr;
}

double AOmniRoad::GetRoadWidth()
{
	if (OB_IS_VALID(PlacedMesh) == false)
		return 0.0;

	const FBox PlaceMeshBox = PlacedMesh->GetBoundingBox();
	return OmniMath::GetBoxWidth(PlaceMeshBox);
}

UOmniConnectorComponent* AOmniRoad::DetectOmniRoad(UOmniConnectorComponent* const Connector)
{
	const uint8 AccessPointIdx = Connector->GetAccessPointIdx();
	UClass* const TargetClass  = Connector->StaticClass();

	//겹치는 컴포넌트 추적.
	TArray<UPrimitiveComponent*> OverlappingComps;
	FOmniStatics::GetOverlapComps(Connector, TargetClass, OverlappingComps);
	UPrimitiveComponent** FindPtr = OverlappingComps.FindByPredicate([TargetClass](const UPrimitiveComponent* InOverlap) -> bool
	{
		return InOverlap->IsA(TargetClass);
	});

	// 겹치는게 없다면, 연결된 도로 제거. 있다면 추가함.
	if (FindPtr == nullptr)
	{
		RemoveConnectedRoadBoth(AccessPointIdx);
		return nullptr;
	}

	UOmniConnectorComponent* DetectedTargetConnector = Cast<UOmniConnectorComponent>(*FindPtr);
	AddConnectedRoadBoth(DetectedTargetConnector, Connector->GetAccessPointIdx());
	return DetectedTargetConnector;
}

bool AOmniRoad::FindNearestRoadAndLane(const TArray<AActor*>& InActors, UClass* InTargetClass, const FVector& InTargetPos, AOmniRoad*& OutNearRoad
                                     , uint32& OutLaneIdx)
{
	bool IsFind = false;

	float DistanceFromNearest = TNumericLimits<float>::Max();
	for (auto& InActor : InActors)
	{
		if (IsValid(InActor) && InActor->IsA(InTargetClass))
		{
			AOmniRoad* Road     = Cast<AOmniRoad>(InActor);
			const uint8 LaneNum = Road->GetLaneSplineNum();
			for (int idx = 0; idx < LaneNum; ++idx)
			{
				const USplineComponent* Lane = Road->GetLaneSpline(idx);
				FVector ClosestSplineLoc     = Lane->FindLocationClosestToWorldLocation(InTargetPos, ESplineCoordinateSpace::World);
				const float DistanceFromLane = (InTargetPos - ClosestSplineLoc).SizeSquared();
				if (DistanceFromLane < DistanceFromNearest)
				{
					DistanceFromNearest = DistanceFromLane;

					OutNearRoad = Road;
					OutLaneIdx  = idx;
					IsFind      = true;
				}
			}
		}
	}

	return IsFind;
}
