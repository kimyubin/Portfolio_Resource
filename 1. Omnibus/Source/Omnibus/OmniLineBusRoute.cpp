// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniLineBusRoute.h"

#include "Omnibus.h"
#include "OmnibusGameInstance.h"
#include "OmnibusPlayData.h"
#include "OmnibusUtilities.h"
#include "OmniRoad.h"
#include "OmniVehicleBus.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

AOmniLineBusRoute::AOmniLineBusRoute()
{
	PrimaryActorTick.bCanEverTick = true;

	const FName RoadSplineName = OmniTool::ConcatStrInt("BusRouteSpline", 0);
	BusRouteSpline = CreateDefaultSubobject<USplineComponent>(RoadSplineName);
	BusRouteSpline->SetupAttachment(RootComponent);
	BusRouteSpline->bAllowDiscontinuousSpline = true;    // 도착, 출발 탄젠트를 따로 제어
}

void AOmniLineBusRoute::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateRouteRoad();

	SpawnBus(FTransform(FVector(GetActorLocation().X, GetActorLocation().Y, 0.0)));
}

void AOmniLineBusRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniLineBusRoute::SpawnBus(const FTransform& SpawnTransform)
{
	const TSubclassOf<AOmniVehicleBus> VehicleBusClass = GetGameInstance<UOmnibusGameInstance>()->GetOmnibusPlayData()->GetOmniVehicleBusClass();

	AOmniVehicleBus* NewBus = GetWorld()->SpawnActorDeferred<AOmniVehicleBus>(VehicleBusClass, SpawnTransform);
	NewBus->SetupSpawnInit(this);
	NewBus->FinishSpawning(SpawnTransform);

	MyBuses.Push(NewBus);
}

void AOmniLineBusRoute::GetNearestOmniRoadAndLane(AOmniRoad*& OutNearRoad, uint32& OutLaneIdx) const
{
	OutNearRoad = nullptr;
	OutLaneIdx  = 0;

	TArray<AActor*> OmniRoads;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniRoad::StaticClass(), OmniRoads);

	float DistanceFromNearest = TNumericLimits<float>::Max();
	const FVector ActLoc      = GetActorLocation();
	for (AActor* ActorToCheck : OmniRoads)
	{
		if (IsValid(ActorToCheck) == false)
			continue;

		AOmniRoad* Road     = Cast<AOmniRoad>(ActorToCheck);
		const uint8 LaneNum = Road->GetLaneSplineNum();
		for (int idx = 0; idx < LaneNum; ++idx)
		{
			const USplineComponent* Lane = Road->GetLaneSpline(idx);
			FVector ClosestSplineLoc     = Lane->FindLocationClosestToWorldLocation(ActLoc, ESplineCoordinateSpace::World);
			const float DistanceFromLane = (ActLoc - ClosestSplineLoc).SizeSquared();
			if (DistanceFromLane < DistanceFromNearest)
			{
				OutNearRoad         = Road;
				OutLaneIdx          = idx;
				DistanceFromNearest = DistanceFromLane;
			}
		}
	}
}

void AOmniLineBusRoute::GenerateRouteRoad()
{
	const int32 DefaultSplineNum = BusRouteSpline->GetNumberOfSplinePoints();
	BusRouteRoads.Empty();
	
	// 가까운 도로 및 차선 찾은 후 차선 추가.
	AOmniRoad* FirstRoad  = nullptr;
	AOmniRoad* SecondRoad = nullptr;
	uint32 FirstRoadLaneIdx;

	GetNearestOmniRoadAndLane(FirstRoad, FirstRoadLaneIdx);
	SecondRoad    = FirstRoad->GetConnectedRoad(FirstRoadLaneIdx);
	BusRouteRoads = {FirstRoad, SecondRoad};

	PushToRouteSpline(FirstRoad->GetLaneSpline(FirstRoadLaneIdx));
	// 기본 스플라인 포인트 삭제
	for(int PointIdx = 0; PointIdx < DefaultSplineNum; ++PointIdx)
		BusRouteSpline->RemoveSplinePoint(0);

	constexpr int LoopLimit = 1000;
	for (int i = 0; i < LoopLimit; ++i)
	{
		AOmniRoad* PrevRoad    = BusRouteRoads.Last(1).Get();
		AOmniRoad* CurrentRoad = BusRouteRoads.Last(0).Get();
		AOmniRoad* NextRoad    = nullptr;
		USplineComponent* CurrentLane = nullptr;

		CurrentRoad->GetLaneAndNextLane(PrevRoad, CurrentLane, NextRoad);

		if (OB_IS_VALID(NextRoad) == false || OB_IS_VALID(CurrentLane) == false)
			continue;

		BusRouteRoads.Push(NextRoad);
		PushToRouteSpline(CurrentLane);
		
		// 최초 출발지에 도착하면 탈출(경로를 루프로 구성함)
		if (CurrentRoad != SecondRoad && NextRoad == FirstRoad)
			break;
	}
}

void AOmniLineBusRoute::PushToRouteSpline(const USplineComponent* InAddLaneSpline)
{
	constexpr ESplineCoordinateSpace::Type CoordWorld = ESplineCoordinateSpace::World;

	const int32 AddSplineNum = InAddLaneSpline->GetNumberOfSplinePoints();

	for (int AddPointIdx = 0; AddPointIdx < AddSplineNum; ++AddPointIdx)
	{
		const FVector AddPointLocation  = InAddLaneSpline->GetLocationAtSplinePoint(AddPointIdx, CoordWorld);
		const FVector AddPointTangent   = InAddLaneSpline->GetTangentAtSplinePoint(AddPointIdx, CoordWorld);
		
		BusRouteSpline->AddSplinePoint(AddPointLocation, CoordWorld, true);
		const int32 BusRoutePointIdx = BusRouteSpline->GetNumberOfSplinePoints() - 1;
		BusRouteSpline->SetTangentAtSplinePoint(BusRoutePointIdx, AddPointTangent, CoordWorld);

		
		// Arrive 도착 탄젠트 - 진입, 들어오는 쪽
		// Leave 출발 탄젠트 - 진출, 나가는 쪽
		// InAddLaneSpline의 양끝point의 탄젠트는 스플라인 내부 방향만 유지함.
		FInterpCurvePointVector& CurrentPoint = BusRouteSpline->SplineCurves.Position.Points[BusRoutePointIdx];
		if (AddPointIdx == 0)
		{
			//시작 지점은 도착 탄젠트 크기 0
			CurrentPoint.ArriveTangent = FVector::Zero();
		}
		else if (AddPointIdx == (AddSplineNum - 1))
		{
			//마지막 지점은 출발 탄젠트 크기 0
			CurrentPoint.LeaveTangent = FVector::Zero();
		}
		
		BusRouteSpline->UpdateSpline();
	}
}
