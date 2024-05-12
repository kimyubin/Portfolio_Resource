// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniLineBusRoute.h"

#include "Omnibus.h"
#include "OmnibusGameInstance.h"
#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmnibusUtilities.h"
#include "OmniRoad.h"
#include "OmniRoadDefaultTwoLane.h"
#include "OmniStationBusStop.h"
#include "OmniVehicleBus.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AOmniLineBusRoute::AOmniLineBusRoute()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Stationary);
	

	const FName RoadSplineName = OmniStr::ConcatStrInt("RouteSpline", 0);
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
	
	AOmnibusRoadManager* RoadManager = GetOmnibusRoadManager();
	if (IsValid(RoadManager))
	{
		RoadManager->AddOmniRoute(this);
		RootComponent->SetVisibility(RoadManager->IsRouteVisible(), true);
	}
}

void AOmniLineBusRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniLineBusRoute::MakeRouteAndBus()
{
	LineColor = EOmniColor::GetNextColorByHue(32, 5);
	RouteSpline->SetUnselectedSplineSegmentColor(LineColor);
	
	GenerateRoute();
	MakeRouteSpline();
	
	MakeSplineMeshComponents();

	SpawnBus(FTransform(FVector(GetActorLocation().X, GetActorLocation().Y, 0.0)));
}

void AOmniLineBusRoute::SpawnBus(const FTransform& SpawnTransform)
{
	const TSubclassOf<AOmniVehicleBus> VehicleBusClass = GetOmnibusPlayData()->GetOmniVehicleBusClass();

	AOmniVehicleBus* NewBus = GetWorld()->SpawnActorDeferred<AOmniVehicleBus>(VehicleBusClass
	                                                                        , SpawnTransform, nullptr, nullptr
	                                                                        , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
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

	AOmniRoad::FindNearestRoadAndLane(OmniRoads
	                                , AOmniRoadDefaultTwoLane::StaticClass()
	                                , GetActorLocation()
	                                , OutNearRoad
	                                , OutLaneIdx);
}

void AOmniLineBusRoute::GenerateRoute()
{
	BusRouteRoads.Empty();

	AOmniRoad* FirstRoad  = nullptr;
	AOmniRoad* SecondRoad = nullptr;
	uint32 FirstRoadLaneIdx;

	// 가까운 도로 및 차선 찾은 후 차선 추가.
	GetNearestOmniRoadTwoLaneAndLane(FirstRoad, FirstRoadLaneIdx);
	SecondRoad    = FirstRoad->GetConnectedRoad(FirstRoadLaneIdx);
	BusRouteRoads = {FirstRoad, SecondRoad};

	// 랜덤 기반
	const int LoopLimit = OmniMath::GetIntRandom(4, 17);
	for (int i = 0; i < LoopLimit; ++i)
	{
		AOmniRoad* const PrevRoad     = BusRouteRoads.Last(1).Get();
		AOmniRoad* const CurrentRoad  = BusRouteRoads.Last(0).Get();
		if(OB_IS_VALID(CurrentRoad) == false)
			return;
		
		AOmniRoad* const NextRoad     = CurrentRoad->GetRandomNextRoad(PrevRoad);
		USplineComponent* CurrentLane = nullptr;

		if (IsValid(NextRoad))
			CurrentLane = CurrentRoad->GetSplineToNextRoad(PrevRoad, NextRoad);

		if (IsValid(NextRoad) == false || OB_IS_VALID(CurrentLane) == false)
			continue;

		BusRouteRoads.Push(NextRoad);

		// 최초 출발지에서 출발한 반대 방향으로 들어오면 탈출(경로를 루프로 구성함. 순환선)
		if (CurrentRoad != SecondRoad && NextRoad == FirstRoad)
			break;
	}
	
	if (BusRouteRoads.Last().Get()->IsA(AOmniRoadDefaultTwoLane::StaticClass()) == false)
		BusRouteRoads.Pop();
}

void AOmniLineBusRoute::MakeRouteSpline()
{
	// 기본 스플라인 포인트 삭제
	const int32 DefaultSplineNum = RouteSpline->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < DefaultSplineNum; ++PointIdx)
		RouteSpline->RemoveSplinePoint(0);

	AOmniRoad* const FirstRoad      = BusRouteRoads[0].Get();
	AOmniRoad* const SecondRoad     = BusRouteRoads[1].Get();
	AOmniRoad* const LastSecondRoad = BusRouteRoads.Last(1).Get();
	AOmniRoad* const LastFirstRoad  = BusRouteRoads.Last(0).Get();

	if (OB_IS_VALID(FirstRoad) == false || OB_IS_VALID(SecondRoad) == false)
		return;

	// 시작 도로 차선 추가
	const int32 FirstRoadLaneIdx = FirstRoad->FindConnectedRoadIdx(SecondRoad);
	if (FirstRoadLaneIdx == INDEX_NONE)
		return;
	PushToRouteSpline(FirstRoad->GetLaneSpline(FirstRoadLaneIdx));

	// 마지막 도로 제외.
	for (int idx = 1; idx < BusRouteRoads.Num() - 1; ++idx)
	{
		AOmniRoad* const PrevRoad     = BusRouteRoads[idx - 1].Get();
		AOmniRoad* const CurrentRoad  = BusRouteRoads[idx].Get();
		AOmniRoad* const NextRoad     = BusRouteRoads[idx + 1].Get();
		USplineComponent* CurrentLane = CurrentRoad->GetSplineToNextRoad(PrevRoad, NextRoad);

		if (OB_IS_VALID(CurrentLane) == false)
			continue;

		PushToRouteSpline(CurrentLane);
	}

	// 용어
	// 순환선 - 1. 출발지로 돌아오고, 2. 처음 출발한 방향의 '반대방향'에서 돌아오면서, 전체 루프를 구성.
	// 반환선 - 1. 출발지로 돌아오고, 2. 처음 출발한 방향으로 돌아오면, 순환선이지만, 첫번째==마지막 차선에서 유턴으로 순환해야함.
	// 일반선 - 1. 출발지로 돌아오지 않음. 왔던 길로 되돌아가고, 양끝에서 유턴함. 끝.
	const bool bIsLoop = ((LastFirstRoad == FirstRoad) && (LastSecondRoad != SecondRoad));
	
	// 순환선이 아닌 경우. U턴으로 닫힌 루프 구성
	if (bIsLoop == false)
	{
		// 마지막 도로 차선 추가.
		const int32 LastRoadLaneIdx = LastFirstRoad->FindConnectedRoadIdx(LastSecondRoad);
		if (LastRoadLaneIdx == INDEX_NONE)
			return;
		PushToRouteSpline(LastFirstRoad->GetLaneSpline((LastRoadLaneIdx == 0) ? 1 : 0));

		// 일반선. 왔던 길을 되돌아감.
		if (LastFirstRoad != FirstRoad)
		{
			const int32 RouteRoadsSize    = BusRouteRoads.Num();
			const int32 TurnStartPointIdx = RouteSpline->GetNumberOfSplinePoints() - 1;
			const int32 LastLaneReverseIdx = LastFirstRoad->FindConnectedRoadIdx(LastSecondRoad);
			if (LastLaneReverseIdx == INDEX_NONE)
				return;
			PushToRouteSpline(LastFirstRoad->GetLaneSpline(LastLaneReverseIdx));

			for (int RoadIdx = RouteRoadsSize - 1; RoadIdx >= 2; --RoadIdx)
			{
				AOmniRoad* PrevRoad    = BusRouteRoads[RoadIdx].Get();
				AOmniRoad* CurrentRoad = BusRouteRoads[RoadIdx - 1].Get();
				AOmniRoad* NextRoad    = BusRouteRoads[RoadIdx - 2].Get();

				const USplineComponent* CurrentLane = CurrentRoad->GetSplineToNextRoad(PrevRoad, NextRoad);

				if (OB_IS_VALID(NextRoad) == false || OB_IS_VALID(CurrentLane) == false)
					continue;

				PushToRouteSpline(CurrentLane);
			}
			MakeUTurnRouteSpline(TurnStartPointIdx, TurnStartPointIdx + 1);

			// 마지막 도로 차선 추가(첫번째 도로)
			const uint32 ReverseLaneIdx         = (FirstRoadLaneIdx == 0) ? 1 : 0;
			const USplineComponent* CurrentLane = BusRouteRoads[0]->GetLaneSpline(ReverseLaneIdx);
			if (OB_IS_VALID(CurrentLane))
			{
				PushToRouteSpline(CurrentLane);
			}
		}
		
		// 출발한 도로 끝에서 U턴으로 닫힌 루프 구성.
		// 반환선, 일반선.
		MakeUTurnRouteSpline(RouteSpline->GetNumberOfSplinePoints() - 1, 0);
	}

	RouteSpline->SetClosedLoop(true);
}

void AOmniLineBusRoute::MakeSplineMeshComponents()
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;
	constexpr bool bManualAttachment = false;
	const bool bVisible = GetOmnibusRoadManager()->IsRouteVisible();

	// 색조값(Hue == R)으로 렌더링 순서(=높이) 결정. 360이 최대값이므로 0~10으로 정규화를 위해 36으로 나눔.
	const double OffsetHue = LineColor.FromRGBE().LinearRGBToHSV().R / 36.0;
	const FVector Offset   = FVector(0.0, 0.0, 10.0 + OffsetHue);

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

		if (OB_IS_VALID(NewSplineMesh) == false)
			continue;

		NewSplineMesh->SetStaticMesh(PlacedMesh);
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
			NewMatInstanceDynamic->SetVectorParameterValue("Color", FLinearColor(LineColor));
			NewSplineMesh->SetMaterial(0, NewMatInstanceDynamic);
		}

		FinishAddComponent(NewSplineMesh, bManualAttachment, FTransform());
	}
}

void AOmniLineBusRoute::PushToRouteSpline(const USplineComponent* InAddLaneSpline)
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	const int32 AddSplineNum = InAddLaneSpline->GetNumberOfSplinePoints();

	for (int AddPointIdx = 0; AddPointIdx < AddSplineNum; ++AddPointIdx)
	{
		const FVector AddPointLocation = InAddLaneSpline->GetLocationAtSplinePoint(AddPointIdx, CoordSpace);
		FVector AddPointArriveTangent  = InAddLaneSpline->GetArriveTangentAtSplinePoint(AddPointIdx, CoordSpace);
		FVector AddPointLeaveTangent   = InAddLaneSpline->GetLeaveTangentAtSplinePoint(AddPointIdx, CoordSpace);

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
	// 평균에서 후퇴한 비율을 좌우 차선에 대입해서 후퇴. 해당 지점에서 U턴.
	const double HalfLaneSpacing    = (StartPointPos - EndPointPos).Length() / 2.0;
	const double StartSectionDist   = std::abs(StartPointDist - PrevStartPointDist);
	const double EndSectionDist     = std::abs(EndPointDist - NextEndPointDist);
	const double AvgSectionDist     = (StartSectionDist + EndSectionDist) / 2.0;
	const double AvgSpacingRate     = HalfLaneSpacing / AvgSectionDist; // 평균 길이 대비, 차선 간격이 차지하는 비율 
	const double AvgBackRate        = 1 - AvgSpacingRate;               // 변화율
	const double TurnStartPointDist = StartPointDist - (StartSectionDist * AvgSpacingRate);
	const double TurnEndPointDist   = EndPointDist + (EndSectionDist * AvgSpacingRate);

	// 변경할 위치. Tangent는 변경 전 접선의 기울기
	const FVector TurnStartPointPos     = RouteSpline->GetLocationAtDistanceAlongSpline(TurnStartPointDist, CoordSpace);
	const FVector TurnEndPointPos       = RouteSpline->GetLocationAtDistanceAlongSpline(TurnEndPointDist, CoordSpace);
	const FVector TurnStartPointTangent = RouteSpline->GetTangentAtDistanceAlongSpline(TurnStartPointDist, CoordSpace) * AvgBackRate;
	const FVector TurnEndPointTangent   = RouteSpline->GetTangentAtDistanceAlongSpline(TurnEndPointDist, CoordSpace) * AvgBackRate;

	RouteSpline->SetLocationAtSplinePoint(InStartPoint, TurnStartPointPos, CoordSpace);
	RouteSpline->SetLocationAtSplinePoint(InEndPoint, TurnEndPointPos, CoordSpace);

	// 좌회전(U턴) 방향에 수직 방향(시계방향 회전. 직진방향)  
	const FVector StartToEndNormal = (TurnEndPointPos - TurnStartPointPos).GetSafeNormal();
	const FVector TurnTangent      = FVector(StartToEndNormal.Y, -StartToEndNormal.X, StartToEndNormal.Z) * HalfLaneSpacing * 4;

	RouteSpline->SetTangentsAtSplinePoint(InStartPoint, TurnStartPointTangent, -1 * TurnTangent   , CoordSpace);
	RouteSpline->SetTangentsAtSplinePoint(InEndPoint  , TurnTangent          , TurnEndPointTangent, CoordSpace);

	RouteSpline->UpdateSpline();
}

void AOmniLineBusRoute::InsertBusStop(AOmniStationBusStop* InFrontBusStop, AOmniStationBusStop* InAddBusStop)
{
	if (InFrontBusStop == nullptr)
	{
		RouteBusStops.Insert(InAddBusStop, 0);
		return;
	}
	const int32 FindIdx = RouteBusStops.Find(InFrontBusStop);
	if (FindIdx == INDEX_NONE)
		return;

	if (FindIdx == (RouteBusStops.Num() - 1))
	{
		RouteBusStops.Emplace(InAddBusStop);
		return;
	}

	RouteBusStops.Insert(InAddBusStop, FindIdx + 1);
}

void AOmniLineBusRoute::SetRouteRender(const bool SetVisibility)
{
	RootComponent->SetVisibility(SetVisibility, true);
}

void AOmniLineBusRoute::ToggleRouteRender()
{
	RootComponent->ToggleVisibility(true);
}

