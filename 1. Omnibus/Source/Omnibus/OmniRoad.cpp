// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniRoad.h"

#include <random>

#include "Omnibus.h"
#include "OmnibusUtilities.h"
#include "OmniDetectSphereComponent.h"
#include "OmniStationBusStop.h"
#include "Components/ArrowComponent.h"
#include "OmniLaneApproachCollision.h"
#include "Components/SplineComponent.h"

AOmniRoad::AOmniRoad()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OmniRoadRoot"));

	// PlacedMesh 기본값 세팅
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultPlacedMesh(TEXT("/Game/Meshs/RoadVertex.RoadVertex"));
	if (DefaultPlacedMesh.Succeeded())
		PlacedMesh = DefaultPlacedMesh.Object;

	InitArrays(0, 0, 0);
}

void AOmniRoad::BeginPlay()
{
	Super::BeginPlay();
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

void AOmniRoad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DetectAllConnectedOmniRoad();
	
	BoxCollisionExtent = FVector(32.0, GetRoadWidth() / 12, 32.0);
	BoxCollisionOffset = BoxCollisionExtent.X * 1 * -1;
}

void AOmniRoad::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	SnapActorLocation();
	DetectAllConnectedOmniRoad();
}

void AOmniRoad::BeginDestroy()
{
	RemoveAllConnectedRoadsBoth();

	Super::BeginDestroy();
}

void AOmniRoad::DetectAllConnectedOmniRoad()
{
	// 현재 도로의 감지용 컴포넌트 순회. 감지된 대상 도로를 찾음.
	for (UOmniDetectSphereComponent* const Detector : RoadConnectDetectors)
	{
		if (OB_IS_VALID(Detector) == false)
			continue;

		DetectOmniRoad(Detector);
	}
}

void AOmniRoad::SnapActorLocation()
{
	FVector TempActorLocation = GetActorLocation();
	if (TempActorLocation.Z != 0.0)
	{
		OB_LOG_STR("Note. Locking to z-axis 0.0")
		TempActorLocation.Z = 0.0;
	}
	
	SetActorLocation(OmniMath::RoundHalfToEvenVector(TempActorLocation, FOmniConst::Unit_Length));
}


void AOmniRoad::InitArrays(const uint8 InRoadSplineNum, const uint8 InRoadConnectDetectorNum, const uint8 InLaneSplineNum)
{
	RoadSplineNum          = InRoadSplineNum;
	RoadConnectDetectorNum = InRoadConnectDetectorNum;
	LaneSplineNum          = InLaneSplineNum;

	ConnectedRoadsArray.Empty();
	RoadConnectDetectors.Empty();
	RoadSplines.Empty();
	LaneSplines.Empty();
	Debug_LaneArrows.Empty();

	ConnectedRoadsArray.SetNum(RoadConnectDetectorNum);
	RoadConnectDetectors.SetNum(RoadConnectDetectorNum);
	RoadSplines.SetNum(RoadSplineNum);
	LaneSplines.SetNum(LaneSplineNum);
	Debug_LaneArrows.SetNum(LaneSplineNum);
}

void AOmniRoad::InitRoadSpline(const uint32 InIdx)
{
	check(RoadSplines.IsValidIndex(InIdx))

	const FName RoadSplineName            = OmniTool::ConcatStrInt("RoadSpline", InIdx);
	USplineComponent* const CurRoadSpline = CreateDefaultSubobject<USplineComponent>(RoadSplineName);

	if (OB_IS_VALID(CurRoadSpline) == false)
		return;

	RoadSplines[InIdx] = CurRoadSpline;
	CurRoadSpline->SetupAttachment(RootComponent);
}

void AOmniRoad::InitRoadConnectDetector(const uint32 InIdx, USplineComponent* InOwnerRoadSpline, const uint32 InSplinePointIdx)
{
	check(RoadConnectDetectors.IsValidIndex(InIdx))
	if (OB_IS_VALID(InOwnerRoadSpline) == false)
		return;

	const FName ConnectorName                     = OmniTool::ConcatStrInt("RoadConnectDetector", InIdx);
	UOmniDetectSphereComponent* const CurDetector = CreateDefaultSubobject<UOmniDetectSphereComponent>(ConnectorName);

	if (OB_IS_VALID(CurDetector) == false)
		return;

	RoadConnectDetectors[InIdx] = CurDetector;
	CurDetector->SetupAttachment(InOwnerRoadSpline);
	CurDetector->SetOwnerOmniRoadAndSpline(this, InOwnerRoadSpline, InSplinePointIdx, InIdx);
}

void AOmniRoad::InitLaneSpline(const uint32 InIdx, USplineComponent* InOwnerRoadSpline)
{
	check(LaneSplines.IsValidIndex(InIdx))
	if (OB_IS_VALID(InOwnerRoadSpline) == false)
		return;

	const FName LaneSplineName            = OmniTool::ConcatStrInt("LaneSpline", InIdx);
	USplineComponent* const CurLaneSpline = CreateDefaultSubobject<USplineComponent>(LaneSplineName);
	if (OB_IS_VALID(CurLaneSpline) == false)
		return;

	LaneSplines[InIdx] = CurLaneSpline;
	CurLaneSpline->SetupAttachment(InOwnerRoadSpline);

	////////////////////
	// 디버그용 화살표
	check(Debug_LaneArrows.IsValidIndex(InIdx))

	const FName DebugArrowName               = OmniTool::ConcatStrInt("DebugArrow", InIdx);
	UArrowComponent* const CurDebugLaneArrow = CreateDefaultSubobject<UArrowComponent>(DebugArrowName);
	if (OB_IS_VALID(CurDebugLaneArrow) == false)
		return;

	Debug_LaneArrows[InIdx] = CurDebugLaneArrow;
	CurDebugLaneArrow->SetHiddenInGame(true);
	CurDebugLaneArrow->bIsEditorOnly = true;
	CurDebugLaneArrow->ArrowSize     = 2.f;
	CurDebugLaneArrow->ArrowColor    = FColor::Purple;
	CurDebugLaneArrow->SetupAttachment(CurLaneSpline);
}

USplineComponent* AOmniRoad::GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	OB_LOG("omniRoad에서 로드됨")
	if (OB_IS_VALID(InNextTargetRoad))
	{
		const int PrevRoadIdx = FindConnectedRoadIdx(InPrevRoad);
		const int NextRoadIdx = FindConnectedRoadIdx(InNextTargetRoad);
		if (NextRoadIdx != INDEX_NONE)
		{
			return nullptr;
		}
	}

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

UArrowComponent* AOmniRoad::GetDebugLaneArrow(const uint32 InIdx) const
{
	check(Debug_LaneArrows.IsValidIndex(InIdx))

	if (IsValid(Debug_LaneArrows[InIdx]))
		return Debug_LaneArrows[InIdx];

	return nullptr;
}

void AOmniRoad::AddConnectedRoadSingle(AOmniRoad* InRoad, const uint8 InAccessIdx)
{
	if (OB_IS_VALID(InRoad) && (InRoad->GetOmniID() != GetOmniID()))
	{
		if (ConnectedRoadsArray.IsValidIndex(InAccessIdx))
			ConnectedRoadsArray[InAccessIdx] = InRoad;
		else
			OB_LOG_STR("InAccessIdx : invalid value")
	}
}

void AOmniRoad::AddConnectedRoadBoth(const UOmniDetectSphereComponent* InTargetDetector, const uint8 InMyAccessIdx)
{
	AOmniRoad* const TargetRoad = InTargetDetector->GetOwnerOmniRoad();
	const uint8 TargetAccessIdx = InTargetDetector->GetAccessPointIdx();
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

int32 AOmniRoad::FindConnectedRoadIdx(AOmniRoad* InRoad) const
{
	return ConnectedRoadsArray.Find(InRoad);
}

bool AOmniRoad::HasOwnedBusStop(const uint64 InId)
{
	const AOmniStationBusStop* FindResult = OmniContainer::TMap_Find(OwnedBusStops, InId);

	return (FindResult != nullptr);
}

void AOmniRoad::AddOwnedBusStop(AOmniStationBusStop* InBusStop)
{
	if (OB_IS_VALID(InBusStop))
	{
		InBusStop->SetOwnerOmniRoad(this);
		OmniContainer::TMap_Emplace(OwnedBusStops, InBusStop->GetOmniID(), InBusStop);
	}
}

void AOmniRoad::RemoveOwnedBusStop(const uint64 InId)
{
	OwnedBusStops.Remove(InId);
}

void AOmniRoad::RemoveOwnedBusStop(AOmniStationBusStop* InBusStop)
{
	if (OB_IS_VALID(InBusStop))
		RemoveOwnedBusStop(InBusStop->GetOmniID());
}

AOmniStationBusStop* AOmniRoad::FindOwnedBusStop(const uint64 InId)
{
	return OmniContainer::TMap_Find(OwnedBusStops, InId);
}

void AOmniRoad::FindPath(AOmniRoad* InStartRoad, AOmniRoad* InEndRoad, AOmniRoad* PrevRoad, TArray<AOmniRoad*>& OutPath)
{
}

AOmniRoad* AOmniRoad::GetRandomConnectedRoad(AOmniRoad* PrevRoad)
{
	if (PrevRoad == nullptr)
		return ConnectedRoadsArray[0];

	std::random_device deviceSeed;
	std::mt19937 rdEngine(deviceSeed());
	std::uniform_int_distribution<int> rdRange(0, ConnectedRoadsArray.Num() - 1);

	const int32 PrevRoadIdx = FindConnectedRoadIdx(PrevRoad);
	int32 RdIdx(0);
	constexpr int LoopLimit = 1000;
	for (int i = 0; i < LoopLimit; ++i)
	{
		RdIdx = rdRange(rdEngine);
		if ((RdIdx != PrevRoadIdx) && IsValid(ConnectedRoadsArray[RdIdx]))
			break;
	}

	return ConnectedRoadsArray[RdIdx];
}

AOmniRoad* AOmniRoad::GetRandomNextRoad(AOmniRoad* InPrevRoad)
{
	const int32 PrevRoadIdx = FindConnectedRoadIdx(InPrevRoad);

	std::random_device deviceSeed;
	std::mt19937 rdEngine(deviceSeed());
	std::uniform_int_distribution<int> rdRange(0, ConnectedRoadsArray.Num() - 1);

	constexpr int LoopLimit = 1000;
	for (int i = 0; i < LoopLimit; ++i)
	{
		const int32 RdIdx = rdRange(rdEngine);
		if ((RdIdx != PrevRoadIdx) && ConnectedRoadsArray.IsValidIndex(RdIdx) && OB_IS_VALID(ConnectedRoadsArray[RdIdx]))
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

UOmniDetectSphereComponent* AOmniRoad::DetectOmniRoad(UOmniDetectSphereComponent* const Detector)
{
	const uint8 AccessPointIdx = Detector->GetAccessPointIdx();
	UClass* const TargetClass  = Detector->StaticClass();

	//겹치는 컴포넌트 추적.
	TArray<UPrimitiveComponent*> OverlappingComps = Detector->GetOverlapComps(TargetClass);
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

	UOmniDetectSphereComponent* DetectedTargetSphere = Cast<UOmniDetectSphereComponent>(*FindPtr);
	AddConnectedRoadBoth(DetectedTargetSphere, Detector->GetAccessPointIdx());
	return DetectedTargetSphere;
}
