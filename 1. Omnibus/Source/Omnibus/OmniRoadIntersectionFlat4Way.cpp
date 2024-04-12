// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniRoadIntersectionFlat4Way.h"

#include "OmnibusUtilities.h"
#include "OmnibusTypes.h"

#include "OmniDetectSphereComponent.h"
#include "Components/ArrowComponent.h"
#include "OmniLaneApproachCollision.h"
#include "Components/SplineComponent.h"

// Sets default values
AOmniRoadIntersectionFlat4Way::AOmniRoadIntersectionFlat4Way()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitArrays(4, 2, 4, 12);

	for (int i = 0; i < RoadSplineNum; ++i)
	{
		InitRoadSpline(i);
	}

	for (int i = 0; i < RoadConnectDetectorNum; ++i)
	{
		const uint32 RoadSplineIdx = i / 2;
		if (RoadSplines.IsValidIndex(RoadSplineIdx) == false)
		{
			OB_LOG("RoadSplines Invalid Index %d", RoadSplineIdx)
			return;
		}
		
		USplineComponent* RoadSpline = RoadSplines[RoadSplineIdx];
		InitRoadConnectDetector(i, RoadSpline, i % 2);
	}

	for (int i = 0; i < LaneSplineNum; ++i)
	{
		InitLaneSpline(i, GetRoadSpline(i / 6));
	}

	for (int i = 0; i < AccessPointNum; ++i)
	{
		InitLaneApproachBox(i, GetLaneSpline(i * 3));
	}

	const FName Mesh = OmniTool::ConcatStrInt("Flat4WayMesh", 0);
	Flat4WayMesh     = CreateDefaultSubobject<UStaticMeshComponent>(Mesh);
	Flat4WayMesh->SetupAttachment(RootComponent);

	if (OB_IS_VALID(PlacedMesh))
		Flat4WayMesh->SetStaticMesh(PlacedMesh);
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

void AOmniRoadIntersectionFlat4Way::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetLanePoints();
	SetSplinesTransform();
	SetCompTransform();
}

void AOmniRoadIntersectionFlat4Way::SetSplinesTransform()
{
	if (OB_IS_VALID(Flat4WayMesh) == false || OB_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
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

	for (UOmniDetectSphereComponent* const Detector : RoadConnectDetectors)
	{
		if (OB_IS_VALID(Detector))
			Detector->SetRelativeTransformToSpline();
	}

	const uint32 LaneMaxIdx = LaneSplineNum / 3; // 12/3 = 4

	for (uint32 i = 0; i < LaneMaxIdx; ++i)
	{
		const int LaneIdx  = i * 3; // 4개 방향 출발지 마다 3개 차선씩(좌,우회전, 직진 차선). 차선 12개. 3개 묶어서 계산.
		const int PointIdx = i * 2; // 4개 방향 마다 출입로 2개씩 진출입 포인트 총 8개. 그 중 진입로는 번갈아가면서 등장하므로 건너뜀(2칸).

		const uint32 EndLeftPointIdx     = OmniMath::CircularNum(LanePoints.Num() - 1, PointIdx + 3);   // 좌회전 - 시계 방향으로 3칸 후.
		const uint32 EndStraightPointIdx = OmniMath::CircularNum(LanePoints.Num() - 1, PointIdx - 3);   // 직진 - 시계 방향으로 3칸 전.
		const uint32 EndRightPointIdx    = OmniMath::CircularNum(LanePoints.Num() - 1, PointIdx - 1);   // 우회전 - 시계 방향으로 1칸 전.
		check(LanePoints.IsValidIndex(EndLeftPointIdx))
		check(LanePoints.IsValidIndex(EndStraightPointIdx))
		check(LanePoints.IsValidIndex(EndRightPointIdx))

		const FVector StartPoint       = LanePoints[PointIdx];
		const FVector EndLeftPoint     = LanePoints[EndLeftPointIdx];
		const FVector EndStraightPoint = LanePoints[EndStraightPointIdx];
		const FVector EndRightPoint    = LanePoints[EndRightPointIdx];

		//각 변을 4등분. 좌회전은 전방 3/4 지점, 우회전은 전방 1/4 지점. 탄젠트 크기는 실제 위치보다 2배.
		FVector      StartTangentNormal = (EndStraightPoint - StartPoint).GetSafeNormal();
		const double LeftTangentSize    = (RoadWidth / 4.0) * 3.0 * 2.0;
		const double RightTangentSize   = (RoadWidth / 4.0) * 2.0;

		TArray<FVector> EndPoints    = {EndLeftPoint, EndStraightPoint, EndRightPoint};
		TArray<double>  TangentSizes = {LeftTangentSize, RoadWidth, RightTangentSize};
		for (int EndPointIdx = 0; EndPointIdx < EndPoints.Num(); ++EndPointIdx)
		{
			USplineComponent* const LaneSpline = GetLaneSpline(LaneIdx + EndPointIdx);
			if (OB_IS_VALID(LaneSpline) == false)
				continue;

			FVector StartTangent = StartTangentNormal * TangentSizes[EndPointIdx];
			LaneSpline->SetLocationAtSplinePoint(0, StartPoint, CoordSpace);
			LaneSpline->SetLocationAtSplinePoint(1, EndPoints[EndPointIdx], CoordSpace);
			LaneSpline->SetTangentAtSplinePoint(0, StartTangent, CoordSpace);

			FVector EndPointTangent_RelativeLocation = StartPoint + StartTangent / 2 - EndPoints[EndPointIdx];
			FVector EndPointTangent                  = EndPointTangent_RelativeLocation * -2.0;

			LaneSpline->SetTangentAtSplinePoint(1, EndPointTangent, CoordSpace);
		}
	}
}

void AOmniRoadIntersectionFlat4Way::SetCompTransform()
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	for (int i = 0; i < Lane_ApproachBoxes.Num(); ++i)
	{
		UOmniLaneApproachCollision* const LaneApproach     = GetLaneApproachBox(i);
		const USplineComponent*           TargetLaneSpline = GetLaneSpline(i * 3);
		if (OB_IS_VALID(LaneApproach) == false || OB_IS_VALID(TargetLaneSpline) == false)
			continue;

		LaneApproach->SetBoxExtent(BoxCollisionExtent);

		const FVector    Lane_StartLoc       = TargetLaneSpline->GetLocationAtSplinePoint(0, CoordSpace);
		const FVector    Lane_StartDirection = TargetLaneSpline->GetDirectionAtSplinePoint(0, CoordSpace);
		const FTransform ApproachTransform   = OmniMath::GetTransformAddOffset(Lane_StartLoc, Lane_StartDirection, BoxCollisionOffset);

		LaneApproach->SetRelativeTransform(ApproachTransform);

		// 디버그 화살표
		UArrowComponent* const LaneArrow = GetDebugLaneArrow(i);
		if (OB_IS_VALID(LaneArrow) == false)
			continue;
		
		LaneArrow->SetRelativeLocation(Lane_StartLoc);
		LaneArrow->SetRelativeRotation(Lane_StartDirection.Rotation());
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

USplineComponent* AOmniRoadIntersectionFlat4Way::GetLaneByApproachIdx(const uint32 ApproachIdx, const uint8 LaneDirection) const
{
	const uint32 LaneMaxIdx = LaneSplineNum / 4; // 12/4 = 3, 0~3
	const uint32 LandIdx    = OmniMath::CircularNum(LaneSplineNum - 1, ApproachIdx * LaneMaxIdx + LaneDirection);
	
	return GetLaneSpline(LandIdx);
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetLaneByApproachIdx(const uint32 ApproachIdx, const ERoadDirection& LaneDirection) const
{
	return GetLaneByApproachIdx(ApproachIdx, EnumToInt(LaneDirection));
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetLaneByApproachIdx(const ERoadApproach& ApproachIdx, const ERoadDirection& LaneDirection) const
{
	return GetLaneByApproachIdx(EnumToInt(ApproachIdx), EnumToInt(LaneDirection));
}

double AOmniRoadIntersectionFlat4Way::GetRoadWidth()
{
	if (OB_IS_VALID(Flat4WayMesh) == false || OB_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return 10.0; // 오류 방지 기본 값.

	if (OB_IS_VALID(PlacedMesh) == false)
		PlacedMesh = Flat4WayMesh->GetStaticMesh();

	const FBox PlaceMeshBox = PlacedMesh->GetBoundingBox();
	return OmniMath::GetBoxWidth(PlaceMeshBox);
}

FIntersectionDimensionInfo AOmniRoadIntersectionFlat4Way::GetIntersectionDimensionInfo()
{
	FIntersectionDimensionInfo Result = FIntersectionDimensionInfo();

	if (OB_IS_VALID(Flat4WayMesh) == false || OB_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return Result;

	const double RoadWidth = GetRoadWidth();
	Result.bIsMeshValid    = true;
	Result.RoadWidth       = RoadWidth;
	Result.X_BasePoint     = FVector(RoadWidth / 2.0, 0.0, 0.0);
	Result.Y_BasePoint     = FVector(0.0, RoadWidth / 2.0, 0.0);

	return Result;
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetSplineToNextRoad(const int32 InLaneApproachIdx, AOmniRoad* InNextTargetRoad)
{
	if (OB_IS_VALID(InNextTargetRoad))
	{
		const int NextRoadIdx = FindConnectedRoadIdx(InNextTargetRoad);
		if (NextRoadIdx != INDEX_NONE)
		{
			const ERoadDirection Direction = (GetLaneDirectionByConnectedIdx(InLaneApproachIdx, NextRoadIdx));
			return GetLaneByApproachIdx(InLaneApproachIdx, Direction);
		}
	}

	return nullptr;
}

USplineComponent* AOmniRoadIntersectionFlat4Way::GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	if (OB_IS_VALID(InNextTargetRoad))
	{
		const int PrevRoadIdx = FindConnectedRoadIdx(InPrevRoad);
		const int NextRoadIdx = FindConnectedRoadIdx(InNextTargetRoad);
		if (PrevRoadIdx != INDEX_NONE && NextRoadIdx != INDEX_NONE)
		{
			const ERoadDirection Direction = (GetLaneDirectionByConnectedIdx(PrevRoadIdx, NextRoadIdx));
			return GetLaneByApproachIdx(PrevRoadIdx, Direction);
		}
	}

	return nullptr;
}


void AOmniRoadIntersectionFlat4Way::AddConnectedRoadSingle(AOmniRoad* InRoad, const uint8 InAccessIdx)
{
	if (OB_IS_VALID(InRoad) && (InRoad->GetOmniID() != GetOmniID()))
	{
		const uint8 ConvertAccessIdx = ConvertDetectorIdxToConnectRoadIdx(InAccessIdx);
		
		if (ConnectedRoadsArray.IsValidIndex(ConvertAccessIdx))
			ConnectedRoadsArray[ConvertAccessIdx] = InRoad;
		else
			OB_LOG_STR("InAccessIdx : invalid value")
	}
}

ERoadDirection AOmniRoadIntersectionFlat4Way::GetLaneDirectionByConnectedIdx(const uint32 StartLaneApproachIdx, const uint32 TargetRoadIdx) const
{
	check(Lane_ApproachBoxes.IsValidIndex(StartLaneApproachIdx))
	check(ConnectedRoadsArray.IsValidIndex(TargetRoadIdx))

	ERoadDirection Direction      = ERoadDirection::Straight;
	const uint32 DirectionNumeric = OmniMath::CircularNum(EnumToInt(ERoadDirection::Size), (TargetRoadIdx - StartLaneApproachIdx) - 1);

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

uint32 AOmniRoadIntersectionFlat4Way::ConvertDetectorIdxToConnectRoadIdx(const uint32 InDetectorIdx)
{
    /**
     * RoadConnectDetectors의 위치가 부모 RoadSpline의 양 끝에 위치함.
     * Idx가 시계방향이 아니라, 0 3 1 2 순서라서 변환해야함.
     *      0            0
     *   2     3  =>  3     1
     *      1            2
     */
	uint32 ResConnectRoadIdx = InDetectorIdx;
	switch (InDetectorIdx)
	{
		case 0: ResConnectRoadIdx = 0; break;
		case 3: ResConnectRoadIdx = 1; break;
		case 1: ResConnectRoadIdx = 2; break;
		case 2: ResConnectRoadIdx = 3; break;
		default: break;
	}
	return ResConnectRoadIdx;
}

uint32 AOmniRoadIntersectionFlat4Way::ConvertConnectRoadIdxToDetectorIdx(const uint32 InConnectRoadIdx)
{
	/**
	 * RoadConnectDetectors의 위치가 부모 RoadSpline의 양 끝에 위치함.
	 * Idx가 시계방향이 아니라, 0 3 1 2 순서라서 변환해야함.
	 *      0            0
	 *   2     3  <=  3     1
	 *      1            2
	 */
	uint32 ResConnectRoadIdx = InConnectRoadIdx;
	switch (InConnectRoadIdx)
	{
		case 0: ResConnectRoadIdx = 0; break;
		case 1: ResConnectRoadIdx = 3; break;
		case 2: ResConnectRoadIdx = 1; break;
		case 3: ResConnectRoadIdx = 2; break;
		default: break;
	}
	return ResConnectRoadIdx;
}
