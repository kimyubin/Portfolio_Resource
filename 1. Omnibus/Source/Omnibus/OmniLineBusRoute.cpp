// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniLineBusRoute.h"

#include "Omnibus.h"
#include "OmnibusGameInstance.h"
#include "OmnibusPlayData.h"
#include "OmnibusUtilities.h"
#include "OmniRoad.h"
#include "OmniRoadDefaultTwoLane.h"
#include "OmniVehicleBus.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AOmniLineBusRoute::AOmniLineBusRoute()
{
	PrimaryActorTick.bCanEverTick = true;

	const FName RoadSplineName = OmniTool::ConcatStrInt("RouteSpline", 0);
	RouteSpline = CreateDefaultSubobject<USplineComponent>(RoadSplineName);
	RouteSpline->SetupAttachment(RootComponent);
	RouteSpline->bAllowDiscontinuousSpline = true;    // 도착, 출발 탄젠트를 따로 제어
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

void AOmniLineBusRoute::GetNearestOmniRoadTwoLaneAndLane(AOmniRoad*& OutNearRoad, uint32& OutLaneIdx) const
{
	OutNearRoad = nullptr;
	OutLaneIdx  = 0;

	TArray<AActor*> OmniRoads;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniRoadDefaultTwoLane::StaticClass(), OmniRoads);

	float DistanceFromNearest = TNumericLimits<float>::Max();
	const FVector ActLoc      = GetActorLocation();
	for (AActor* ActorToCheck : OmniRoads)
	{
		if (IsValid(ActorToCheck) == false)
			continue;

		AOmniRoadDefaultTwoLane* Road     = Cast<AOmniRoadDefaultTwoLane>(ActorToCheck);
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
	const int32 DefaultSplineNum = RouteSpline->GetNumberOfSplinePoints();
	BusRouteRoads.Empty();
	
	bool bIsLoopRoute = false;
	AOmniRoad* FirstRoad  = nullptr;
	AOmniRoad* SecondRoad = nullptr;
	uint32 FirstRoadLaneIdx;

	// 가까운 도로 및 차선 찾은 후 차선 추가.
	GetNearestOmniRoadTwoLaneAndLane(FirstRoad, FirstRoadLaneIdx);
	SecondRoad    = FirstRoad->GetConnectedRoad(FirstRoadLaneIdx);
	BusRouteRoads = {FirstRoad, SecondRoad};
	PushToRouteSpline(FirstRoad->GetLaneSpline(FirstRoadLaneIdx));

	// 기본 스플라인 포인트 삭제
	for(int PointIdx = 0; PointIdx < DefaultSplineNum; ++PointIdx)
		RouteSpline->RemoveSplinePoint(0);

	// 랜덤 기반
	constexpr int LoopLimit = 10;
	for (int i = 0; i < LoopLimit; ++i)
	{
		AOmniRoad* const PrevRoad     = BusRouteRoads.Last(1).Get();
		AOmniRoad* const CurrentRoad  = BusRouteRoads.Last(0).Get();
		// todo: 랜덤 기반에서 목적지 길찾기 기반으로 변경해야함.
		AOmniRoad* const NextRoad     = CurrentRoad->GetRandomNextRoad(PrevRoad);
		USplineComponent* CurrentLane = nullptr;

		if(OB_IS_VALID(NextRoad))
			CurrentLane = CurrentRoad->GetSplineToNextRoad(PrevRoad, NextRoad);

		if (OB_IS_VALID(NextRoad) == false || OB_IS_VALID(CurrentLane) == false)
			continue;

		BusRouteRoads.Push(NextRoad);
		PushToRouteSpline(CurrentLane);
		
		// 최초 출발지에서 출발한 반대 방향으로 들어오면 탈출(경로를 루프로 구성함. 순환선)
		if (CurrentRoad != SecondRoad && NextRoad == FirstRoad)
		{
			bIsLoopRoute = true;
			break;
		}
	}

	// 경로가 순환선이 아닌 경우. U턴으로 닫힌 루프 구성
	if (bIsLoopRoute == false)
	{
		// 출발지로 돌아오지 않은 경우. 왔던 길을 되돌아감.
		if (BusRouteRoads[0].Get() != BusRouteRoads.Last(0).Get())
		{
			const int RouteRoadsSize    = BusRouteRoads.Num();
			const int32 UturnStartPoint = RouteSpline->GetNumberOfSplinePoints() - 1;
			
			for(int RoadIdx = RouteRoadsSize - 1; RoadIdx >= 2 ; --RoadIdx)
			{
				AOmniRoad* PrevRoad    = BusRouteRoads[RoadIdx].Get();
				AOmniRoad* CurrentRoad = BusRouteRoads[RoadIdx - 1].Get();
				AOmniRoad* NextRoad    = BusRouteRoads[RoadIdx - 2].Get();

				const USplineComponent* CurrentLane = CurrentRoad->GetSplineToNextRoad(PrevRoad, NextRoad);

				if (OB_IS_VALID(NextRoad) == false || OB_IS_VALID(CurrentLane) == false)
					continue;

				PushToRouteSpline(CurrentLane);
			}
			MakeUTurnRouteSpline(UturnStartPoint, UturnStartPoint + 1);
		}
		
		// 출발한 도로 끝에서 U턴으로 닫힌 루프 구성.
		// 출발지로 돌아오되, 나온 방향으로 다시 들어온 경우 포함. 
		const uint32 ReverseLaneIdx         = (FirstRoadLaneIdx == 0) ? 1 : 0;
		const USplineComponent* CurrentLane = BusRouteRoads[0]->GetLaneSpline(ReverseLaneIdx);
		if (OB_IS_VALID(CurrentLane))
		{
			PushToRouteSpline(CurrentLane);
			MakeUTurnRouteSpline(RouteSpline->GetNumberOfSplinePoints() - 1, 0);
		}
		
	}


	for (auto& Road : BusRouteRoads)
	{
		TempBusRouteRoads.Push(Road.Get());
	}
	
	RouteSpline->SetClosedLoop(true);
}

void AOmniLineBusRoute::PushToRouteSpline(const USplineComponent* InAddLaneSpline)
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	const int32 AddSplineNum = InAddLaneSpline->GetNumberOfSplinePoints();

	for (int AddPointIdx = 0; AddPointIdx < AddSplineNum; ++AddPointIdx)
	{
		const FVector AddPointLocation      = InAddLaneSpline->GetLocationAtSplinePoint(AddPointIdx, CoordSpace);
		const FVector AddPointArriveTangent = InAddLaneSpline->GetArriveTangentAtSplinePoint(AddPointIdx, CoordSpace);
		const FVector AddPointLeaveTangent  = InAddLaneSpline->GetLeaveTangentAtSplinePoint(AddPointIdx, CoordSpace);

		RouteSpline->AddSplinePoint(AddPointLocation, CoordSpace, true);
		const int32 BusRoutePointIdx = RouteSpline->GetNumberOfSplinePoints() - 1;
		
		RouteSpline->SplineCurves.Position.Points[BusRoutePointIdx].ArriveTangent = AddPointArriveTangent;
		RouteSpline->SplineCurves.Position.Points[BusRoutePointIdx].LeaveTangent  = AddPointLeaveTangent;
		RouteSpline->SetSplinePointType(BusRoutePointIdx, ConvertInterpCurveModeToSplinePointType(CIM_CurveUser));

		// InAddLaneSpline의 양끝point의 탄젠트는 스플라인 내부 방향만 유지함.
		FInterpCurvePointVector& CurrentPoint = RouteSpline->SplineCurves.Position.Points[BusRoutePointIdx];
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
		
		RouteSpline->UpdateSpline();
	}
}

void AOmniLineBusRoute::MakeUTurnRouteSpline(const int32 InStartPoint, const int32 InEndPoint)
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	// 초기값
	const FVector StartPointPos    = RouteSpline->GetLocationAtSplinePoint(InStartPoint, CoordSpace);
	const FVector EndPointPos      = RouteSpline->GetLocationAtSplinePoint(InEndPoint, CoordSpace);
	const float StartPointDist     = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InStartPoint);
	const float EndPointDist       = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InEndPoint);
	const float PrevStartPointDist = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InStartPoint - 1);
	const float NextEndPointDist   = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InEndPoint + 1);

	// 두 차선의 직전 구간 길이의 평균에서 차선 간격(절반)만큼 후퇴.
	// 평균에서 후퇴한 비율을 좌우 차선에 대입해서 후퇴. 같은 비율만큼 후퇴. 해당 지점에서 U턴.
	const double HalfLaneSpacing    = (StartPointPos - EndPointPos).Length() / 2.0;
	const double StartSectionDist   = std::abs(StartPointDist - PrevStartPointDist);
	const double EndSectionDist     = std::abs(EndPointDist - NextEndPointDist);
	const double AvgSectionDist     = (StartSectionDist + EndSectionDist) / 2.0;
	const double AvgSpacingRate     = HalfLaneSpacing / AvgSectionDist;
	const double TurnStartPointDist = StartPointDist - (StartSectionDist * AvgSpacingRate);
	const double TurnEndPointDist   = EndPointDist + (EndSectionDist * AvgSpacingRate);

	
	// 변경할 위치. Tangent는 변경 전 접선의 기울기
	const FVector TurnStartPointPos     = RouteSpline->GetLocationAtDistanceAlongSpline(TurnStartPointDist, CoordSpace);
	const FVector TurnEndPointPos       = RouteSpline->GetLocationAtDistanceAlongSpline(TurnEndPointDist, CoordSpace);
	const FVector TurnStartPointTangent = RouteSpline->GetTangentAtDistanceAlongSpline(TurnStartPointDist, CoordSpace);
	const FVector TurnEndPointTangent   = RouteSpline->GetTangentAtDistanceAlongSpline(TurnEndPointDist, CoordSpace);

	RouteSpline->SetLocationAtSplinePoint(InStartPoint, TurnStartPointPos, CoordSpace);
	RouteSpline->SetLocationAtSplinePoint(InEndPoint, TurnEndPointPos, CoordSpace);

	// 변화율
	const double StartRate = (TurnStartPointDist - PrevStartPointDist) / (StartPointDist - PrevStartPointDist);
	const double EndRate   = (TurnEndPointDist - NextEndPointDist) / (EndPointDist - NextEndPointDist);

	// U턴하는 방향과 그 수직하는 방향의 노말. 수직 노말 방향을 U턴 구역의 Tangent로 사용함.  
	const FVector StartToEndNormal  = (TurnEndPointPos - TurnStartPointPos).GetSafeNormal();
	const FVector TurnTangentNormal = FVector(StartToEndNormal.Y, -StartToEndNormal.X, StartToEndNormal.Z);

	FInterpCurvePointVector& StartPointCurve = RouteSpline->SplineCurves.Position.Points[InStartPoint];
	StartPointCurve.ArriveTangent            = TurnStartPointTangent * StartRate;
	StartPointCurve.LeaveTangent             = -1 * TurnTangentNormal * HalfLaneSpacing * 4;

	FInterpCurvePointVector& EndPointCurve = RouteSpline->SplineCurves.Position.Points[InEndPoint];
	EndPointCurve.ArriveTangent            = TurnTangentNormal * HalfLaneSpacing * 4;
	EndPointCurve.LeaveTangent             = TurnEndPointTangent * EndRate;

	RouteSpline->UpdateSpline();
}

