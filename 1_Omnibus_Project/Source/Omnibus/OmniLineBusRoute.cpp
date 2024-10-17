// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniLineBusRoute.h"

#include "OmniAsync.h"
#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmnibusTypes.h"
#include "OmniRoad.h"
#include "OmniStationBusStop.h"
#include "OmniVehicleBus.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "UtlLog.h"
#include "UTLStatics.h"

#include <queue>
#include <unordered_map>

AOmniLineBusRoute::AOmniLineBusRoute()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Stationary);

	const FName RoadSplineName = UtlStr::ConcatStrInt(TEXT("RouteSpline"), 0);
	RouteSpline = CreateDefaultSubobject<USplineComponent>(RoadSplineName);
	RouteSpline->SetupAttachment(RootComponent);

#if WITH_EDITORONLY_DATA
	RouteSpline->bAllowDiscontinuousSpline = true; // 도착, 출발 탄젠트를 따로 제어
#endif // WITH_EDITORONLY_DATA

	RootComponent->SetVisibility(false, true);
}

void AOmniLineBusRoute::BeginPlay()
{
	Super::BeginPlay();

	LineColor = FOmniColor::GetNextColorByHue(32, 5);
	RouteSpline->SetUnselectedSplineSegmentColor(LineColor);

	// 노선 데이터 등록
	FOmniAsync::UpdateLineProxyAsync(this);

	// 비긴플레이 : 마우스 클릭으로 스폰 시.
	AOmnibusRoadManager* RoadManager = GetOmnibusRoadManager();
	if (IsValid(RoadManager))
	{
		RoadManager->AddOmniRoute(this);
		RootComponent->SetVisibility(RoadManager->IsRouteVisible(), true);
	}
}

void AOmniLineBusRoute::PostBeginPlay()
{
	Super::PostBeginPlay();

	// 버스 노선이 레벨에 미리 스폰되어져 있을 때 사용. 필요 없어지면 제거 예정.
	AOmnibusRoadManager* RoadManager = GetOmnibusRoadManager();
	if (IsValid(RoadManager))
	{
		RoadManager->AddOmniRoute(this);
		RootComponent->SetVisibility(RoadManager->IsRouteVisible(), true);
	}
}

void AOmniLineBusRoute::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 노선 데이터 삭제
	FOmniAsync::DeleteLineProxyAsync(this);
	
	ClearBusStopList();
	Super::EndPlay(EndPlayReason);
}

void AOmniLineBusRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

TArray<FLaneInfo> AOmniLineBusRoute::FindLanePathByWayPoints(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InBusStops)
{
	if (InBusStops.Num() < 2)
	{
		UT_LOG("Too little BusStops")
		return {};
	}

	TArray<FLaneInfo> OutLaneInfos;

	for (int RouteIdx = 0; RouteIdx < InBusStops.Num() - 1; ++RouteIdx)
	{
		TArray<FLaneInfo> TempLaneInfos = FindLanePath(InBusStops[RouteIdx].Get(), InBusStops[RouteIdx + 1].Get());
		UT_IF(TempLaneInfos.Num() <= 0 || TempLaneInfos.Num() > 1'000)
			return {};

		// 중복제거
		if (OutLaneInfos.IsEmpty() == false && OutLaneInfos.Last() == TempLaneInfos[0])
			OutLaneInfos.Pop();

		OutLaneInfos.Append(MoveTemp(TempLaneInfos));
	}

	// 출발지와 도착지 같으면 순환선
	if (InBusStops[0] == InBusStops.Last())
	{
		// 중복 제거
		if (OutLaneInfos[0] == OutLaneInfos.Last())
			OutLaneInfos.Pop();
	}
	else
	{
		// 출발지와 도착지가 다르면 유턴해서 처음으로 되돌아가는 경로 구성
		// 역방향 생성
		TArray<FLaneInfo> ReverseRoads = OutLaneInfos;
		Algo::Reverse(ReverseRoads);
		TArray<FLaneInfo> NewReversePaths = ReconstructLanePathByRoads(ReverseRoads);

		// 역방향 추가 전에 U턴 추가(회차지점)
		OutLaneInfos.Emplace(FLaneInfo::UTurnInfo());
		OutLaneInfos.Append(MoveTemp(NewReversePaths));

		//중복 제거 후 U턴 추가(시작지점)
		if (OutLaneInfos[0] == OutLaneInfos.Last())
			OutLaneInfos.Pop();

		OutLaneInfos.Emplace(FLaneInfo::UTurnInfo());
	}

	return OutLaneInfos;
}

TArray<FLaneInfo> AOmniLineBusRoute::FindLanePath(const AOmniStationBusStop* InStartBusStop, const AOmniStationBusStop* InEndBusStop)
{
	// A* 알고리즘 기반.
	TArray<FLaneInfo> OutPath;

	const FLaneInfo StartLaneInfo = {InStartBusStop->GetOwnerOmniRoad(), static_cast<int32>(InStartBusStop->GetOwnerLaneIdx())};
	const FLaneInfo EndLaneInfo   = {InEndBusStop->GetOwnerOmniRoad(), static_cast<int32>(InEndBusStop->GetOwnerLaneIdx())};
	AOmniRoad* StartRoad          = InStartBusStop->GetOwnerOmniRoad();
	AOmniRoad* EndRoad            = InEndBusStop->GetOwnerOmniRoad(); 

	if (StartRoad == EndRoad)
	{
		// 동일 차선인 경우, 해당 도로만 반환(해당 도로 지나가는걸로 처리)
		const int32 StartStopLane = InStartBusStop->GetOwnerLaneIdx();
		const int32 EndStopLane   = InEndBusStop->GetOwnerLaneIdx();
		if (StartStopLane == EndStopLane)
		{
			OutPath.Emplace(StartRoad, StartStopLane);
			return OutPath;
		}
		// 반대 차선인 경우 U턴
		OutPath = {{StartRoad, StartStopLane}, {StartRoad, FLaneInfo::UTURN_LANE}, {StartRoad, EndStopLane}};
		return OutPath;
	}

	//~=============================================================================
	// Lambda

	// 목적지 위치 미리 캡쳐.
	// 휴리스틱. 목적지 까지의 예상 거리 계산. 유클리드 거리.
	const FVector2D EndStopPos = FVector2D(InEndBusStop->GetActorLocation());
	auto HeuristicToEndRoad    = [EndStopPos](const FLaneInfo& InNextLaneInfo) -> double
	{
		const FVector2D InLaneStartPos = InNextLaneInfo.GetEndPoint2D(); // 도착지 끝점 기준.
		return (EndStopPos - InLaneStartPos).Size();
	};

	// 최단 경로 재구성 함수
	auto ReconstructPath = [](const std::unordered_map<FLaneInfo, FLaneInfo>& InCameFrom, FLaneInfo InCurrent, TArray<FLaneInfo>& InOutPath)
	{
		while (InCameFrom.contains(InCurrent))
		{
			InOutPath.Push(InCurrent);
			InCurrent = InCameFrom.at(InCurrent);
			if (InOutPath.Num() > 1'000'000)
			{
				UT_LOG("Loop Max Over")
				InOutPath.Empty();
				return;
			}
		}
		InOutPath.Push(InCurrent); // 시작 추가

		Algo::Reverse(InOutPath);
	};

	struct pair_greater
	{
		constexpr bool operator()(const std::pair<double, FLaneInfo>& InLeft, const std::pair<double, FLaneInfo>& InRight) const
		{
			return InLeft.first > InRight.first;
		}
	};
	// Lambda finish.
	//==================

	// F = G + H
	// G = 현재 지점까지 실제 거리
	// H = 현재 지점에서 남은 예상 거리
	std::priority_queue<std::pair<double, FLaneInfo>, std::vector<std::pair<double, FLaneInfo>>, pair_greater> PriorityQ;
	std::unordered_map<FLaneInfo, FLaneInfo> CameFrom;      // 경로 추적을 위한 맵. cameFrom[Second] = Start;는 Second에 도달하려면 직전에 Start를 거친다는 의미
	std::unordered_map<FLaneInfo, double> F_DistStartToEnd; // G(실제거리) + 휴리스틱 예상 남은 거리
	std::unordered_map<FLaneInfo, double> G_DistFromStart;  // 시작 노드로부터 현재까지 실제 거리

	PriorityQ.emplace(0.0, StartLaneInfo);
	F_DistStartToEnd[StartLaneInfo] = HeuristicToEndRoad(StartLaneInfo);
	G_DistFromStart[StartLaneInfo]  = 0.0;

	while (PriorityQ.empty() == false)
	{
		FLaneInfo CurrentLaneInfo = PriorityQ.top().second;
		PriorityQ.pop();

		if (CurrentLaneInfo == EndLaneInfo)
		{
			ReconstructPath(CameFrom, CurrentLaneInfo, OutPath);
			return OutPath;
		}

		TArray<FLaneInfo> NextLaneInfos = CurrentLaneInfo.GetNextLaneInfos();
		for (FLaneInfo& NextPath : NextLaneInfos)
		{
			// NextPath가 목적지면 바로 종료.
			// NextPath의 EndPoint와 비교하기 때문에, 우선순위q에 넣기 전에 먼저 비교함.
			if (NextPath == EndLaneInfo)
			{
				CameFrom[NextPath] = CurrentLaneInfo;
				ReconstructPath(CameFrom, NextPath, OutPath);
				return OutPath;
			}

			double tentativeNextGDist = G_DistFromStart[CurrentLaneInfo] + CurrentLaneInfo.GetLength(); // 다음 도로가는 차선 비용
			if (G_DistFromStart.contains(NextPath) == false || tentativeNextGDist < G_DistFromStart[NextPath])
			{
				CameFrom[NextPath]         = CurrentLaneInfo;
				G_DistFromStart[NextPath]  = tentativeNextGDist;
				F_DistStartToEnd[NextPath] = G_DistFromStart[NextPath] + HeuristicToEndRoad(NextPath);

				PriorityQ.emplace(F_DistStartToEnd[NextPath], NextPath);
			}
		}
	}

	return {};
}

TArray<FLaneInfo> AOmniLineBusRoute::ReconstructLanePathByRoads(const TArray<FLaneInfo>& InRouteRoads)
{
	TArray<FLaneInfo> OutNewLanePath;

	AOmniRoad* const FirstRoad      = InRouteRoads[0].OmniRoad.Get();
	AOmniRoad* const SecondRoad     = InRouteRoads[1].OmniRoad.Get();
	AOmniRoad* const LastSecondRoad = InRouteRoads.Last(1).OmniRoad.Get();
	AOmniRoad* const LastFirstRoad  = InRouteRoads.Last(0).OmniRoad.Get();

	if (UT_IS_VALID(FirstRoad) == false || UT_IS_VALID(SecondRoad) == false)
		return OutNewLanePath;

	// 시작 도로 차선 추가.
	const int32 FirstRoadLaneIdx = FirstRoad->FindLaneIdxToNextRoad(nullptr, SecondRoad);
	UT_IF (FirstRoadLaneIdx == INDEX_NONE)
		return OutNewLanePath;

	OutNewLanePath.Emplace(FirstRoad, FirstRoadLaneIdx);

	// 마지막 도로 제외.
	for (int idx = 1; idx < InRouteRoads.Num() - 1; ++idx)
	{
		AOmniRoad* const PrevRoad    = InRouteRoads[idx - 1].OmniRoad.Get();
		AOmniRoad* const CurrentRoad = InRouteRoads[idx].OmniRoad.Get();
		AOmniRoad* const NextRoad    = InRouteRoads[idx + 1].OmniRoad.Get();

		if (PrevRoad != NextRoad)
		{
			const int32 CurrentLaneIdx = CurrentRoad->FindLaneIdxToNextRoad(PrevRoad, NextRoad);
			UT_IF (CurrentLaneIdx == INDEX_NONE)
				return OutNewLanePath;

			OutNewLanePath.Emplace(CurrentRoad, CurrentLaneIdx);
		}
		else
		{
			// 2차선이라는 전제로 진행.
			int32 NextLaneIdx = CurrentRoad->FindLaneIdxToNextRoad(nullptr, NextRoad);
			int32 PrevLaneIdx = (NextLaneIdx == 0 ? 1 : 0);

			OutNewLanePath.Emplace(CurrentRoad, PrevLaneIdx);
			OutNewLanePath.Emplace(CurrentRoad, FLaneInfo::UTURN_LANE);
			OutNewLanePath.Emplace(CurrentRoad, NextLaneIdx);
		}
	}

	const int32 LastRoadLaneIdx = LastFirstRoad->FindLaneIdxToNextRoad(nullptr, LastSecondRoad);
	UT_IF(LastRoadLaneIdx == INDEX_NONE)
		return OutNewLanePath;

	OutNewLanePath.Emplace(LastFirstRoad, (LastRoadLaneIdx == 0) ? 1 : 0);
	return OutNewLanePath;
}

void AOmniLineBusRoute::InitializeRoute(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InWayPointList)
{
	WayPointStopList = InWayPointList;

	LaneInfoRoute = FindLanePathByWayPoints(WayPointStopList);

	UpdateRouteSpline();

	FOmniAsync::UpdateLineProxyAsync(this);
}

void AOmniLineBusRoute::InitRoutePathAndBus(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InWayPointList)
{
	InitializeRoute(InWayPointList);

	// check(BusStopDistanceList.IsEmpty()==false)
	UT_IF (BusStopDistanceList.IsEmpty())
		return;

	SpawnBusOnRoute(ECarType::Blue_Basic, BusStopDistanceList[0].Distance - 500.0f);
}

void AOmniLineBusRoute::InsertWayPointInRoute(const int32 InFrontWayPointIdx, AOmniStationBusStop* InAddBusStop)
{
	check(WayPointStopList.IsValidIndex(InFrontWayPointIdx));

	//todo : 경로 변경에 따라 없어진 정류장에서 내리는 승객에게 하차 요청 전파하기 필요
#if 1
	// 1안 : 전체 다시 찾기.
	WayPointStopList.Insert(InAddBusStop, InFrontWayPointIdx + 1);
	LaneInfoRoute = FindLanePathByWayPoints(WayPointStopList);

	UpdateRouteSpline();

	FOmniAsync::UpdateLineProxyAsync(this);

#else
	// 2안 : 부분만 찾기. 예외처리가 필요할 수 있음.
	UT_IF (WayPointStopList.IsValidIndex(InFrontWayPointIdx) == false)
		return;

	AOmniStationBusStop* FrontBusStop = WayPointStopList[InFrontWayPointIdx].Get();
	UT_IF (FrontBusStop == nullptr)
		return;
	UT_IF (FrontBusStop == InAddBusStop)
		return;

	const int BackBusStopIdx         = UtlMath::CircularNum(WayPointStopList.Num() - 1, InFrontWayPointIdx + 1);
	AOmniStationBusStop* BackBusStop = WayPointStopList[BackBusStopIdx].Get();
	UT_IF (BackBusStop == InAddBusStop)
		return;;


	const AOmniRoad* FrontRoad = FrontBusStop->GetOwnerOmniRoad();
	const AOmniRoad* BackRoad  = BackBusStop->GetOwnerOmniRoad();

	// 버스 정류장 앞에 동일한 정류장이 몇개 있는지 탐색
	int PrevSameCount = 0;
	for (int idx = 0; idx < InFrontWayPointIdx; ++idx)
	{
		if (WayPointStopList[idx] == FrontBusStop)
			PrevSameCount++;
	}

	// Front 버스 정류장이 있는 LaneInfo의 인덱스. 동일한 경로가 있는 경우를 대비 
	int FrontPathIdx = 0;
	for (; FrontPathIdx < LaneInfoRoute.Num(); ++FrontPathIdx)
	{
		if (LaneInfoRoute[FrontPathIdx].OmniRoad == FrontRoad)
		{
			if (PrevSameCount == 0)
				break;

			PrevSameCount--;
		}
	}

	// 다음 정류장을 포함하는 LaneInfo찾기 
	// 뭔가 이상하지만, 순환 마지막-처음 연결을 고려.
	// FrontPathIdx+1 ~ Last() ~ [0] ~ FrontPathIdx-1 까지 탐색.
	int BackPathIdx = (FrontPathIdx + 1);
	for (; BackPathIdx != FrontPathIdx; BackPathIdx = UtlMath::CircularNum(LaneInfoRoute.Num() - 1, ++BackPathIdx))
	{
		if (LaneInfoRoute[BackPathIdx].OmniRoad == BackRoad)
			break;
	}

	WayPointStopList.Insert(InAddBusStop, InFrontWayPointIdx + 1);

	TArray<FLaneInfo> TempRoutePaths = FindLanePathByWayPoints({FrontBusStop, InAddBusStop, BackBusStop});
	if (FrontPathIdx < BackPathIdx)
	{
		LaneInfoRoute.RemoveAt(FrontPathIdx, BackPathIdx - FrontPathIdx + 1);
		LaneInfoRoute.Insert(MoveTemp(TempRoutePaths), FrontPathIdx);
	}
	else
	{
		LaneInfoRoute.RemoveAt(FrontPathIdx, LaneInfoRoute.Num() - FrontPathIdx);
		LaneInfoRoute.RemoveAt(0, BackPathIdx + 1);
		LaneInfoRoute.Append(MoveTemp(TempRoutePaths));
	}
	
	UpdateRouteSpline();

	FOmniAsync::UpdateLineProxyAsync(this);
#endif
}

void AOmniLineBusRoute::SpawnBusOnRoute(const ECarType InSpawnCarType, const float InSpawnRouteDist)
{
	const double CircularDist = UtlMath::CircularNumF(RouteSpline->GetSplineLength(), InSpawnRouteDist);
	const FVector Location    = RouteSpline->GetLocationAtDistanceAlongSpline(CircularDist, ESplineCoordinateSpace::World);
	const FVector Direction   = RouteSpline->GetDirectionAtDistanceAlongSpline(CircularDist, ESplineCoordinateSpace::World);
	
	// 회전은 Yaw만 반영. 다른 축은 -0.0 등 미세한 오류가 포함되어 있을 수 있으므로 무시.
	const FRotator Rotator    = UtlMath::YawRotator(Direction.Rotation());

	const FTransform SpawnTrans = FTransform(Rotator, Location);
	SpawnBusByTransform(SpawnTrans, InSpawnCarType, InSpawnRouteDist);
}

void AOmniLineBusRoute::SpawnBusByTransform(const FTransform& SpawnTransform, const ECarType InSpawnCarType, const float InSpawnRouteDist)
{
	const TSubclassOf<AOmniVehicleBus> VehicleBusClass = GetOmnibusPlayData()->GetOmniVehicleBusClass();

	AOmniVehicleBus* NewBus = GetWorld()->SpawnActorDeferred<AOmniVehicleBus>(VehicleBusClass
	                                                                        , SpawnTransform, nullptr, nullptr
	                                                                        , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	// todo: 차종 임시 지정 중.
	NewBus->SetupSpawnInit(this, InSpawnCarType, InSpawnRouteDist);
	NewBus->FinishSpawning(SpawnTransform);

	MyBuses.Push(NewBus);
}

void AOmniLineBusRoute::UpdateRouteSpline()
{
	// 버스 정류장 목록 초기화.
	ClearBusStopList();

	// 기본 스플라인 포인트 삭제
	const int32 DefaultSplineNum = RouteSpline->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < DefaultSplineNum; ++PointIdx)
		RouteSpline->RemoveSplinePoint(0);

	// 차선 버스 추가 및 U턴 구역 체크
	std::vector<int32> UturnPoints;
	for (FLaneInfo& Path : LaneInfoRoute)
	{
		if (Path.LaneIdx == FLaneInfo::UTURN_LANE)
		{
			UturnPoints.emplace_back(RouteSpline->GetNumberOfSplinePoints() - 1);
			continue;
		}

		PushLaneAndBusStops(Path);
	}
	for (const int32 UturnPoint : UturnPoints)
	{
		MakeUTurnRouteSpline(UturnPoint, UtlMath::CircularNum(RouteSpline->GetNumberOfSplinePoints() - 1, UturnPoint + 1));
	}

	RouteSpline->SetClosedLoop(true);

	CreateSplineMeshComponents();

	FOmniAsync::UpdateLineProxyAsync(this);
}

void AOmniLineBusRoute::PushLaneAndBusStops(const FLaneInfo& InLaneInfo)
{
	auto [AddLaneSpline, BusStopsOnLane] = InLaneInfo.OmniRoad->GetLaneAndBusStop(InLaneInfo.LaneIdx);
	if (UT_IS_VALID(AddLaneSpline) == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	const int32 AddSplineNum = AddLaneSpline->GetNumberOfSplinePoints();
	for (int AddPointIdx = 0; AddPointIdx < AddSplineNum; ++AddPointIdx)
	{
		const FVector AddPointLocation = AddLaneSpline->GetLocationAtSplinePoint(AddPointIdx, CoordSpace);
		FVector AddPointArriveTangent  = AddLaneSpline->GetArriveTangentAtSplinePoint(AddPointIdx, CoordSpace);
		FVector AddPointLeaveTangent   = AddLaneSpline->GetLeaveTangentAtSplinePoint(AddPointIdx, CoordSpace);

		// InAddLaneSpline의 양끝point의 탄젠트는 스플라인 내부 방향만 유지함.
		if (AddPointIdx == 0)
		{
			//시작 지점은 도착 탄젠트 크기 0
			AddPointArriveTangent = FVector::Zero();
		}
		else if (AddPointIdx == (AddSplineNum - 1))
		{
			//마지막 지점은 출발 탄젠트 크기 0
			AddPointLeaveTangent = FVector::Zero();
		}
		
		RouteSpline->AddSplinePoint(AddPointLocation, CoordSpace, true);
		const int32 BusRoutePointIdx = RouteSpline->GetNumberOfSplinePoints() - 1;

		RouteSpline->SetTangentsAtSplinePoint(BusRoutePointIdx, AddPointArriveTangent, AddPointLeaveTangent, CoordSpace);
	}

	// 정류장 추가.
	AppendBusStopDist(BusStopsOnLane, AddLaneSpline->GetSplineLength());

	FOmniAsync::UpdateLineProxyAsync(this);
}

void AOmniLineBusRoute::AppendBusStopDist(TArray<FBusStopDistance>& InOutAddBusStopList, const float InAddedLaneLength)
{
	if (InOutAddBusStopList.IsEmpty())
		return;

	// 유턴 구역에서는 스플라인 포인트가 점프를 하기 때문에, 이에 대해 대응. 
	const float RouteLength = RouteSpline->GetDistanceAlongSplineAtSplinePoint(RouteSpline->GetNumberOfSplinePoints() - 1) - InAddedLaneLength;
	for (FBusStopDistance& InBusStopDist : InOutAddBusStopList)
	{
		AOmniStationBusStop* BusStop = InBusStopDist.BusStop.Get();
		if (BusStop == nullptr)
			continue;

		BusStop->AddBusRoute(this);
		InBusStopDist.Distance += RouteLength;
	}

	BusStopDistanceList.Append(InOutAddBusStopList);

	FOmniAsync::UpdateLineProxyAsync(this);
}

void AOmniLineBusRoute::MakeUTurnRouteSpline(const int32 InStartPoint, const int32 InEndPoint)
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	const float PrevLastPointLength = RouteSpline->GetDistanceAlongSplineAtSplinePoint(RouteSpline->GetNumberOfSplinePoints() - 1);
	
	// 초기값
	const FVector StartPointPos    = RouteSpline->GetLocationAtSplinePoint(InStartPoint, CoordSpace);
	const FVector EndPointPos      = RouteSpline->GetLocationAtSplinePoint(InEndPoint, CoordSpace);
	const float StartPointDist     = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InStartPoint);
	const float EndPointDist       = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InEndPoint);
	const float PrevStartPointDist = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InStartPoint - 1);
	const float NextEndPointDist   = RouteSpline->GetDistanceAlongSplineAtSplinePoint(InEndPoint + 1);

	// 두 차선의 직전 구간 길이의 평균에서 차선 간격(절반)만큼 후퇴.
	// 평균에서 후퇴한 비율을 좌우 차선에 대입해서 후퇴. 해당 지점에서 U턴.
	const float HalfLaneSpacing    = static_cast<float>((StartPointPos - EndPointPos).Length()) / 2.0f;
	const float StartSectionDist   = std::abs(StartPointDist - PrevStartPointDist);
	const float EndSectionDist     = std::abs(EndPointDist - NextEndPointDist);
	const float AvgSectionDist     = (StartSectionDist + EndSectionDist) / 2.0;
	const float AvgSpacingRate     = HalfLaneSpacing / AvgSectionDist; // 평균 길이 대비, 차선 간격이 차지하는 비율 
	const float AvgBackRate        = 1 - AvgSpacingRate;               // 변화율
	const float TurnStartPointDist = StartPointDist - (StartSectionDist * AvgSpacingRate);
	const float TurnEndPointDist   = EndPointDist + (EndSectionDist * AvgSpacingRate);

	// 변경할 위치. Tangent는 변경 전 접선의 기울기
	const FVector TurnStartPointPos     = RouteSpline->GetLocationAtDistanceAlongSpline(TurnStartPointDist, CoordSpace);
	const FVector TurnEndPointPos       = RouteSpline->GetLocationAtDistanceAlongSpline(TurnEndPointDist, CoordSpace);
	const FVector TurnStartPointTangent = RouteSpline->GetTangentAtDistanceAlongSpline(TurnStartPointDist, CoordSpace) * AvgBackRate;
	const FVector TurnEndPointTangent   = RouteSpline->GetTangentAtDistanceAlongSpline(TurnEndPointDist, CoordSpace) * AvgBackRate;

	RouteSpline->SetLocationAtSplinePoint(InStartPoint, TurnStartPointPos, CoordSpace);
	RouteSpline->SetLocationAtSplinePoint(InEndPoint, TurnEndPointPos, CoordSpace);

	// 좌회전(U턴) 방향에 수직 방향(시계방향 회전. 직진방향)  
	const FVector StartToEndNormal = (TurnEndPointPos - TurnStartPointPos).GetSafeNormal();
	const FVector TurnTangent      = FVector(StartToEndNormal.Y, -StartToEndNormal.X, StartToEndNormal.Z) * (HalfLaneSpacing * 4.0);

	RouteSpline->SetTangentsAtSplinePoint(InStartPoint, TurnStartPointTangent, -1 * TurnTangent   , CoordSpace);
	RouteSpline->SetTangentsAtSplinePoint(InEndPoint  , TurnTangent          , TurnEndPointTangent, CoordSpace);

	RouteSpline->UpdateSpline();

	// StartPointDist 이후에 있는 모든 정류장에 변경된 길이 반영.
	// 마지막 ~ 시작점
	if (InEndPoint == 0)
	{
		// InEndPoint가 시작 지점.
		const double DeltaStartDist = TurnEndPointDist;
		for (FBusStopDistance& BusStopDist : BusStopDistanceList)
		{
			BusStopDist.Distance -= DeltaStartDist;
		}
	}
	else
	{
		const float DeltaLength = RouteSpline->GetDistanceAlongSplineAtSplinePoint(RouteSpline->GetNumberOfSplinePoints() - 1) - PrevLastPointLength;
		for (FBusStopDistance& BusStopDist : BusStopDistanceList)
		{
			if (BusStopDist.Distance > EndPointDist)
				BusStopDist.Distance += DeltaLength;
		}
	}

	FOmniAsync::UpdateLineProxyAsync(this);
}

void AOmniLineBusRoute::CreateSplineMeshComponents()
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;
	constexpr bool bManualAttachment       = false;
	const AOmnibusRoadManager* RoadManager = GetOmnibusRoadManager();
	const bool bVisible                    = RoadManager ? RoadManager->IsRouteVisible() : false;

	UStaticMesh* BusLineMesh = GetOmnibusPlayData()->GetBusLineMesh();

	// 색조값(Hue == R)으로 렌더링 순서(=높이) 결정. 360이 최대값이므로 0~1으로 정규화하기 위해 360으로 나눔.
	const double OffsetHue = LineColor.LinearRGBToHSV().R / 360.0;
	const FVector Offset   = FVector(0.0, 0.0, GetRenderOrderOffset() + OffsetHue);

	// 기존 메시 제거.
	TInlineComponentArray<USplineMeshComponent*> SplineMeshes;
	GetComponents(USplineMeshComponent::StaticClass(), SplineMeshes);
	for (USplineMeshComponent* SplineMesh : SplineMeshes)
	{
		SplineMesh->DestroyComponent();
	}
	
	// 스플라인 컴포넌트 포인트를 따라 스플라인 메시 생성
	const int32 SplinePointsSize = RouteSpline->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < SplinePointsSize; ++PointIdx)
	{
		// 마지막 포인트와 처음 포인트 연결.
		const int NextIdx          = PointIdx + 1 < SplinePointsSize ? PointIdx + 1 : 0;
		const FVector StartLoc     = RouteSpline->GetLocationAtSplinePoint(PointIdx, CoordSpace);
		const FVector StartTangent = RouteSpline->GetLeaveTangentAtSplinePoint(PointIdx, CoordSpace); // 출발 탄젠트
		const FVector EndLoc       = RouteSpline->GetLocationAtSplinePoint(NextIdx, CoordSpace);
		const FVector EndTangent   = RouteSpline->GetArriveTangentAtSplinePoint(NextIdx, CoordSpace); // 도착 탄젠트

		USplineMeshComponent* const NewSplineMesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), bManualAttachment, FTransform(), true));

		if (UT_IS_VALID(NewSplineMesh) == false)
			continue;

		NewSplineMesh->SetStaticMesh(BusLineMesh);
		NewSplineMesh->SetStartAndEnd(StartLoc + Offset, StartTangent, EndLoc + Offset, EndTangent);
		NewSplineMesh->SetVisibility(bVisible);
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		NewSplineMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		NewSplineMesh->SetHiddenInGame(false);
		NewSplineMesh->SetGenerateOverlapEvents(false);
		NewSplineMesh->SetCastShadow(false);

		UMaterialInstanceDynamic* NewMatInstanceDynamic = NewSplineMesh->CreateDynamicMaterialInstance(0, NewSplineMesh->GetMaterial(0));
		if (IsValid(NewMatInstanceDynamic))
		{
			NewMatInstanceDynamic->SetVectorParameterValue(TEXT("Color"), LineColor);
			NewSplineMesh->SetMaterial(0, NewMatInstanceDynamic);
		}

		FinishAddComponent(NewSplineMesh, bManualAttachment, FTransform());
	}

	FOmniAsync::UpdateLineProxyAsync(this);
}

USplineComponent* AOmniLineBusRoute::GetRouteSpline() const
{
	return RouteSpline;
}

float AOmniLineBusRoute::GetRouteLength() const
{
	return GetRouteSpline()->GetSplineLength();
}

TArray<FBusStopDistance> AOmniLineBusRoute::GetBusStopDistanceList() const
{
	return BusStopDistanceList;
}

FBusStopDistance AOmniLineBusRoute::GetBusStopDist(const int32 InIdx) const
{
	UT_IF(BusStopDistanceList.IsValidIndex(InIdx) == false)
		return FBusStopDistance();
	
	return BusStopDistanceList[InIdx];
}

FLinearColor AOmniLineBusRoute::GetLineColor() const
{
	return LineColor;
}

TArray<int32> AOmniLineBusRoute::FindBusStopIdxList(AOmniStationBusStop* InBusStop) const
{
	return UtlContainer::FindAllIndexByPredicate(BusStopDistanceList, [InBusStopWeak = TWeakObjectPtr<AOmniStationBusStop>(InBusStop)](const FBusStopDistance& InStopDistance)
	{
		return InStopDistance.BusStop == InBusStopWeak;
	});
}

float AOmniLineBusRoute::GetShortPathLength(const int32 InStartBusIdx, const int32 InEndBusIdx)
{
	UT_IF(BusStopDistanceList.IsValidIndex(InStartBusIdx) == false || BusStopDistanceList.IsValidIndex(InEndBusIdx) == false)
		return FOmniConst::GetBigNumber<float>();

	// 한바퀴 돈 경우 고려
	float ResultDist = BusStopDistanceList[InEndBusIdx].Distance - BusStopDistanceList[InStartBusIdx].Distance;
	if (ResultDist < 0.0f)
		ResultDist += GetRouteLength();

	return ResultDist;
}

int32 AOmniLineBusRoute::FindThisStopIdxByDist(const float InDist)
{
	const int32 FindIdx = Algo::UpperBoundBy(BusStopDistanceList, InDist, [](const FBusStopDistance& InA)
	{
		return InA.Distance;
	});

	// 마지막 정류장 보다 InDist가 클 경우, 첫번째 정류장을 반환합니다.
	return FindIdx < BusStopDistanceList.Num() ? FindIdx : 0;
}

int32 AOmniLineBusRoute::GetNextBusStopIdx(const int32 InCurrentIdx) const
{
	return UtlMath::CircularNum(BusStopDistanceList.Num() - 1, InCurrentIdx + 1);
}

void AOmniLineBusRoute::UnlinkBusStop(AOmniStationBusStop* InBusStop)
{
	if (IsValid(InBusStop) ==false)
		return;

	InBusStop->RemoveBusRoute(this);
	BusStopDistanceList.RemoveAll([&InBusStop](FBusStopDistance& Element)
	{
		const AOmniStationBusStop* InStop = Element.BusStop.Get();
		return (InStop == nullptr) || (InStop == InBusStop);
	});

	FOmniAsync::UpdateLineProxyAsync(this);
}

void AOmniLineBusRoute::ClearBusStopList()
{
	for (const FBusStopDistance& StopDist : BusStopDistanceList)
	{
		AOmniStationBusStop* BusStop = StopDist.BusStop.Get();
		if (BusStop)
			BusStop->RemoveBusRoute(this);
	}

	BusStopDistanceList.Empty();

	FOmniAsync::UpdateLineProxyAsync(this);
}

float AOmniLineBusRoute::GetRenderOrderOffset()
{
	static float LineOffset = 0.0f;

	return UtlMath::CircularNumF(10.0, LineOffset += 0.1f) + 1.0f;
}

void AOmniLineBusRoute::SetRouteRender(const bool InVisibility)
{
	RootComponent->SetVisibility(InVisibility, true);
}

void AOmniLineBusRoute::ToggleRouteRender()
{
	RootComponent->ToggleVisibility(true);
}

std::tuple<int32, float> AOmniLineBusRoute::GetShortestStartIdxAndDist(const TArray<FBusStopDistance>& InBusStopDistanceList
                                                                     , const float InBusLineLength
                                                                     , const TWeakObjectPtr<AOmniStationBusStop>& InStartBusStop
                                                                     , const int32& InEndBusStopDistanceIdx)
{
	int32 StartStopIdx = INDEX_NONE;
	float ShortestDist = FOmniConst::GetBigNumber<float>(); // 충분히 큰 값

	// BusStopDistanceList에서 버스 정류장이 위치를 모두 찾음.
	// 정류장을 2회 이상 지나가는 것을 고려.
	TArray<int32> StartLaneIdxList = UtlContainer::FindAllIndexByPredicate(InBusStopDistanceList, [InStartBusStop](const FBusStopDistance& InStopDistance)
	{
		return FUtlStatics::IsSameWeak(InStopDistance.BusStop, InStartBusStop);
	});

	UT_IF(StartLaneIdxList.IsEmpty())
		return std::make_tuple(StartStopIdx, ShortestDist);

	for (const int32 StartIdx : StartLaneIdxList)
	{
		// 한바퀴 돈 경우 고려
		float TempDist = InBusStopDistanceList[InEndBusStopDistanceIdx].Distance - InBusStopDistanceList[StartIdx].Distance;
		if (TempDist < 0.0f)
			TempDist += InBusLineLength;

		if (TempDist < ShortestDist)
		{
			StartStopIdx = StartIdx;
			ShortestDist = TempDist;
		}
	}
	return std::make_tuple(StartStopIdx, ShortestDist);
}

void AOmniLineBusRoute::GenerateRandomRoute()
{
	TArray<AActor*> RandBusStops;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniStationBusStop::StaticClass(), RandBusStops);
	if (RandBusStops.IsEmpty())
		return;

	const int32 StopsLastIdx = RandBusStops.Num() - 1;

	AOmniStationBusStop* StartStop = Cast<AOmniStationBusStop>(RandBusStops[UtlMath::GetIntRandom(0, StopsLastIdx)]);
	AOmniStationBusStop* MidStop   = Cast<AOmniStationBusStop>(RandBusStops[UtlMath::GetIntRandom(0, StopsLastIdx)]);
	AOmniStationBusStop* EndStop   = Cast<AOmniStationBusStop>(RandBusStops[UtlMath::GetIntRandom(0, StopsLastIdx)]);

	for (int LoopCount = 0; LoopCount < 1'000; ++LoopCount)
	{
		if (StartStop->GetOwnerOmniRoad() != MidStop->GetOwnerOmniRoad()
			&& MidStop->GetOwnerOmniRoad() != EndStop->GetOwnerOmniRoad()
			&& EndStop->GetOwnerOmniRoad() != StartStop->GetOwnerOmniRoad())
			break;

		if (StartStop->GetOwnerOmniRoad() == MidStop->GetOwnerOmniRoad())
			MidStop = Cast<AOmniStationBusStop>(RandBusStops[UtlMath::GetIntRandom(0, StopsLastIdx)]);

		if (MidStop->GetOwnerOmniRoad() == EndStop->GetOwnerOmniRoad())
			EndStop = Cast<AOmniStationBusStop>(RandBusStops[UtlMath::GetIntRandom(0, StopsLastIdx)]);
	}

	InitRoutePathAndBus({StartStop, MidStop, EndStop, StartStop});
}
