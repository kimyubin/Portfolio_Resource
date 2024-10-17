// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniRoadIntersectionFlat4Way.h"

#include "OmnibusTypes.h"
#include "OmniConnectorComponent.h"
#include "Components/SplineComponent.h"

#include "UtlLog.h"
#include "UTLStatics.h"

// Sets default values
AOmniRoadIntersectionFlat4Way::AOmniRoadIntersectionFlat4Way()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitArrays(2, 4, 12);

	for (int i = 0; i < RoadSplineNum; ++i)
	{
		InitRoadSpline(i);
	}

	for (int ConnectorIdx = 0; ConnectorIdx < RoadConnectorNum; ++ConnectorIdx)
	{
		auto [RoadIdx, RoadSplinePointIdx] = GetConnectorPositionIdx(ConnectorIdx);

		if (RoadSplines.IsValidIndex(RoadIdx) == false)
		{
			UT_LOG("RoadSplines Invalid Index %d", RoadIdx)
			return;
		}
		
		USplineComponent* RoadSpline = RoadSplines[RoadIdx];
		InitRoadConnector(ConnectorIdx, RoadSpline, RoadSplinePointIdx);
	}

	for (int i = 0; i < LaneSplineNum; ++i)
	{
		InitLaneSpline(i, GetRoadSpline(i / 6));
	}

	const FName Mesh = UtlStr::ConcatStrInt(TEXT("Flat4WayMesh"), 0);
	Flat4WayMesh     = CreateDefaultSubobject<UStaticMeshComponent>(Mesh);
	Flat4WayMesh->SetupAttachment(RootComponent);
	Flat4WayMesh->SetMobility(EComponentMobility::Static);
	Flat4WayMesh->SetCastShadow(false);

	TangentSizeRate = 2.0f;
}

void AOmniRoadIntersectionFlat4Way::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (UT_IS_VALID(PlacedMesh))
	{
		Flat4WayMesh->SetStaticMesh(PlacedMesh);
		Flat4WayMesh->SetMobility(EComponentMobility::Static);
		Flat4WayMesh->SetCastShadow(false);
	}

	SetLanePoints();
	SetSplinesTransform();
}

// Called when the game starts or when spawned
void AOmniRoadIntersectionFlat4Way::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOmniRoadIntersectionFlat4Way::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniRoadIntersectionFlat4Way::SetSplinesTransform()
{
	if (UT_IS_VALID(Flat4WayMesh) == false || UT_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;
	
	const FIntersectionDimensionInfo DimensionInfo = GetIntersectionDimensionInfo();

	if (DimensionInfo.bIsMeshValid == false)
		return;

	USplineComponent* const RoadSpline_0 = GetRoadSpline(0);
	USplineComponent* const RoadSpline_1 = GetRoadSpline(1);
	
	const double&  RoadWidth   = DimensionInfo.RoadWidth;
	const FVector& Y_BasePoint = DimensionInfo.Y_BasePoint;
	const FVector& X_BasePoint = DimensionInfo.X_BasePoint;

	//도로 스플라인 위치, 탄젠트 고정.
	RoadSpline_0->SetLocationAtSplinePoint(0, -Y_BasePoint, CoordSpace);
	RoadSpline_0->SetLocationAtSplinePoint(1, Y_BasePoint, CoordSpace);
	RoadSpline_1->SetLocationAtSplinePoint(0, -X_BasePoint, CoordSpace);
	RoadSpline_1->SetLocationAtSplinePoint(1, X_BasePoint, CoordSpace);

	RoadSpline_0->SetTangentAtSplinePoint(0, Y_BasePoint, CoordSpace);
	RoadSpline_0->SetTangentAtSplinePoint(1, Y_BasePoint, CoordSpace);
	RoadSpline_1->SetTangentAtSplinePoint(0, X_BasePoint, CoordSpace);
	RoadSpline_1->SetTangentAtSplinePoint(1, X_BasePoint, CoordSpace);

	for (UOmniConnectorComponent* const Connector : RoadConnectors)
	{
		if (UT_IS_VALID(Connector))
			Connector->SetRelativeTransformToSpline();
	}

	const int32 LaneMaxIdx = LaneSplineNum / 3; // 12/3 = 4

	for (int32 i = 0; i < LaneMaxIdx; ++i)
	{
		const int LaneIdx  = i * 3; // 4개 방향 출발지 마다 3개 차선씩(좌,우회전, 직진 차선). 차선 12개. 3개 묶어서 계산.
		const int PointIdx = i * 2; // 4개 방향 마다 출입로 2개씩 진출입 포인트 총 8개. 그 중 진입로는 번갈아가면서 등장하므로 건너뜀(2칸).

		const int32 EndLeftPointIdx     = UtlMath::CircularNum(LanePoints.Num() - 1, PointIdx + 3);   // 좌회전 - 시계 방향으로 3칸 후.
		const int32 EndStraightPointIdx = UtlMath::CircularNum(LanePoints.Num() - 1, PointIdx - 3);   // 직진 - 시계 방향으로 3칸 전.
		const int32 EndRightPointIdx    = UtlMath::CircularNum(LanePoints.Num() - 1, PointIdx - 1);   // 우회전 - 시계 방향으로 1칸 전.
		check(LanePoints.IsValidIndex(EndLeftPointIdx))
		check(LanePoints.IsValidIndex(EndStraightPointIdx))
		check(LanePoints.IsValidIndex(EndRightPointIdx))

		const FVector StartPoint       = LanePoints[PointIdx];
		const FVector EndLeftPoint     = LanePoints[EndLeftPointIdx];
		const FVector EndStraightPoint = LanePoints[EndStraightPointIdx];
		const FVector EndRightPoint    = LanePoints[EndRightPointIdx];

		FVector      StartTangentNormal = (EndStraightPoint - StartPoint).GetSafeNormal();
		const double LeftTangentSize    = (RoadWidth / 4.0) * 3.0 * TangentSizeRate;
		const double RightTangentSize   = (RoadWidth / 4.0) * TangentSizeRate;

		TArray<FVector> EndPoints    = {EndLeftPoint, EndStraightPoint, EndRightPoint};
		TArray<double>  TangentSizes = {LeftTangentSize, RoadWidth, RightTangentSize};
		for (int EndPointIdx = 0; EndPointIdx < EndPoints.Num(); ++EndPointIdx)
		{
			USplineComponent* const LaneSpline = GetLaneSpline(LaneIdx + EndPointIdx);
			if (UT_IS_VALID(LaneSpline) == false)
				continue;

			// 탄젠트를 상대 위치 취급.
			// 시작 탄젠트를 위치로 치환 후, EndPoint에서의 상대 위치를 탄젠트로 변환.
			FVector StartTangent         = StartTangentNormal * TangentSizes[EndPointIdx];
			FVector StartTangentLocation = StartPoint + StartTangent / TangentSizeRate;
			FVector EndPointTangent      = (StartTangentLocation - EndPoints[EndPointIdx]) * -TangentSizeRate;

			LaneSpline->SetLocationAtSplinePoint(0, StartPoint, CoordSpace);
			LaneSpline->SetLocationAtSplinePoint(1, EndPoints[EndPointIdx], CoordSpace);
			LaneSpline->SetTangentAtSplinePoint(0, StartTangent, CoordSpace);
			LaneSpline->SetTangentAtSplinePoint(1, EndPointTangent, CoordSpace);
		}
	}
}

void AOmniRoadIntersectionFlat4Way::SetLanePoints()
{
	const FIntersectionDimensionInfo DimensionInfo = GetIntersectionDimensionInfo();

	const double&  RoadWidth   = DimensionInfo.RoadWidth;
	const FVector& Y_BasePoint = DimensionInfo.Y_BasePoint;
	const FVector& X_BasePoint = DimensionInfo.X_BasePoint;

	if (DimensionInfo.bIsMeshValid == false)
		return;

	const FVector VectorYOffset = FVector(0.0, RoadWidth / 4.0, 0.0);
	const FVector VectorXOffset = FVector(RoadWidth / 4.0, 0.0, 0.0);
	//좌상단부터 시계방향으로
	LanePoints = {
				-Y_BasePoint - VectorXOffset, -Y_BasePoint + VectorXOffset, X_BasePoint - VectorYOffset, X_BasePoint + VectorYOffset
				, Y_BasePoint + VectorXOffset, Y_BasePoint - VectorXOffset, -X_BasePoint + VectorYOffset, -X_BasePoint - VectorYOffset
				};
}

int32 AOmniRoadIntersectionFlat4Way::CalculateLaneIdx(const int32 ApproachIdx, const uint8 LaneDirection) const
{
	const int32 LaneMaxIdx = LaneSplineNum / 4; // 12/4 = 3, 0~3
	const int32 LaneIdx    = UtlMath::CircularNum(LaneSplineNum - 1, ApproachIdx * LaneMaxIdx + LaneDirection);
	return LaneIdx;
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetLaneByApproachIdx(const int32 ApproachIdx, const uint8 LaneDirection) const
{
	return GetLaneSpline(CalculateLaneIdx(ApproachIdx, LaneDirection));
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetLaneByApproachIdx(const int32 ApproachIdx, const ERoadDirection& LaneDirection) const
{
	return GetLaneByApproachIdx(ApproachIdx, EnumToInt(LaneDirection));
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetLaneByApproachIdx(const ERoadApproach& ApproachIdx, const ERoadDirection& LaneDirection) const
{
	return GetLaneByApproachIdx(EnumToInt(ApproachIdx), EnumToInt(LaneDirection));
}

double AOmniRoadIntersectionFlat4Way::GetRoadWidth()
{
	if (UT_IS_VALID(Flat4WayMesh) == false || UT_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return 10.0; // 오류 방지 기본 값.

	if (UT_IS_VALID(PlacedMesh) == false)
		PlacedMesh = Flat4WayMesh->GetStaticMesh();

	const FBox PlaceMeshBox = PlacedMesh->GetBoundingBox();
	return UtlMath::GetBoxWidth(PlaceMeshBox);
}

FIntersectionDimensionInfo AOmniRoadIntersectionFlat4Way::GetIntersectionDimensionInfo()
{
	FIntersectionDimensionInfo Result = FIntersectionDimensionInfo();

	if (UT_IS_VALID(Flat4WayMesh) == false || UT_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return Result;

	const double RoadWidth = GetRoadWidth();
	Result.bIsMeshValid    = true;
	Result.RoadWidth       = RoadWidth;
	Result.X_BasePoint     = FVector(RoadWidth / 2.0, 0.0, 0.0);
	Result.Y_BasePoint     = FVector(0.0, RoadWidth / 2.0, 0.0);

	return Result;
}

AOmniRoad* AOmniRoadIntersectionFlat4Way::GetNextRoadByLaneIdx(const int32 InLaneIdx)
{
	// (진입도로 번호) 좌 직 우 // 진출 순서임.
	// (0) 1 2 3 (1) 2 3 0 (2) 3 0 1 (3) 0 1 2
	const std::vector<int32> LaneToRoad = {1, 2, 3, 2, 3, 0, 3, 0, 1, 0, 1, 2};
	return GetConnectedRoad(LaneToRoad[InLaneIdx]);
}

int32 AOmniRoadIntersectionFlat4Way::FindLaneIdxToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	// 같은 경우, 잘못된 입력.
	if ((InPrevRoad != InNextTargetRoad) && UT_IS_VALID(InPrevRoad) && IsValid(InNextTargetRoad))
	{
		const int PrevRoadIdx = FindConnectedRoadIdx(InPrevRoad);
		const int NextRoadIdx = FindConnectedRoadIdx(InNextTargetRoad);
		if (PrevRoadIdx != INDEX_NONE && NextRoadIdx != INDEX_NONE)
		{
			const ERoadDirection Direction = (GetLaneDirectionByConnectedIdx(PrevRoadIdx, NextRoadIdx));
			return CalculateLaneIdx(PrevRoadIdx, EnumToInt(Direction));
		}
	}

	return INDEX_NONE;
}

void AOmniRoadIntersectionFlat4Way::AddConnectedRoadSingle(AOmniRoad* InRoad, const int32 InAccessIdx)
{
	if (UT_IS_VALID(InRoad) && (InRoad->GetOmniID() != GetOmniID()))
	{
		if (ConnectedRoadsArray.IsValidIndex(InAccessIdx))
			ConnectedRoadsArray[InAccessIdx] = InRoad;
		else
			UT_LOG("InAccessIdx : invalid value")
	}
}

ERoadDirection AOmniRoadIntersectionFlat4Way::GetLaneDirectionByConnectedIdx(const int32 StartLaneApproachIdx, const int32 TargetRoadIdx) const
{
	check(ConnectedRoadsArray.IsValidIndex(TargetRoadIdx))

	ERoadDirection Direction      = ERoadDirection::Straight;
	const uint32 DirectionNumeric = UtlMath::CircularNum(EnumToInt(ERoadDirection::Size), (TargetRoadIdx - StartLaneApproachIdx) - 1);

	switch (DirectionNumeric)
	{
		case 0:
			Direction = ERoadDirection::Left;
			break;
		case 1:
			Direction = ERoadDirection::Straight;
			break;
		case 2:
			Direction = ERoadDirection::Right;
			break;
		default:
			break;
	}

	return Direction;
}

std::tuple<int32, int32> AOmniRoadIntersectionFlat4Way::GetConnectorPositionIdx(const int32 InConnectorIdx)
{
	/**
     * RoadConnectors의 위치가 부모 RoadSpline의 양 끝에 위치함.
     * Idx가 시계방향이 아니라, 0 3 1 2 순서라서 변환해야함.
     *    0          0           0-0
     * 3     1 => 2     3 => 1-0     1-1
     *    2          1           0-1
	 */
	int32 RoadPointIdx = InConnectorIdx;
	switch (InConnectorIdx)
	{
		case 0: RoadPointIdx = 0; break;
		case 1: RoadPointIdx = 3; break;
		case 2: RoadPointIdx = 1; break;
		case 3: RoadPointIdx = 2; break;
		default: break;
	}

	// 몫==도로 번호, 나머지==SplinePoint
	auto [RoadIdx, SplinePointIdx] = std::div(RoadPointIdx, 2);
	return std::make_tuple(RoadIdx, SplinePointIdx);
}
