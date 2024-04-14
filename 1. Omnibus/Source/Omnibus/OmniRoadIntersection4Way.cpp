// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniRoadIntersection4Way.h"

#include "OmnibusUtilities.h"
#include "OmnibusTypes.h"

#include "OmniDetectSphereComponent.h"
#include "Components/ArrowComponent.h"
#include "OmniLaneApproachCollision.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AOmniRoadIntersection4Way::AOmniRoadIntersection4Way()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitArrays(2, 4, 4);

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
		InitLaneSpline(i, GetRoadSpline(i / 2));
	}

	const FName Mesh = OmniTool::ConcatStrInt("Intersection4WayMesh", 0);
	Flat4WayMesh     = CreateDefaultSubobject<UStaticMeshComponent>(Mesh);
	Flat4WayMesh->SetupAttachment(RootComponent);

	if (OB_IS_VALID(PlacedMesh))
		Flat4WayMesh->SetStaticMesh(PlacedMesh);
}

// Called when the game starts or when spawned
void AOmniRoadIntersection4Way::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOmniRoadIntersection4Way::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniRoadIntersection4Way::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetLanePoints();
	SetSplinesTransform();
	SetCompTransform();
}

void AOmniRoadIntersection4Way::SetSplinesTransform()
{
	if (OB_IS_VALID(Flat4WayMesh) == false || OB_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;
	const FIntersectionDimensionInfo       DimensionInfo     = GetIntersectionDimensionInfo();

	if (DimensionInfo.bIsMeshValid == false)
		return;

	USplineComponent* const RoadSpline_0 = GetRoadSpline(0);
	USplineComponent* const RoadSpline_1 = GetRoadSpline(1);
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

	for (int i = 0; i < LaneSplineNum; ++i)
	{
		const int LaneIdx  = i;
		const int PointIdx = i * 2; // 4개 방향 마다 출입로 2개씩 진출입 포인트 총 8개. 그 중 진입로는 번갈아가면서 등장하므로 건너뜀(2칸). 홀수번째 포인트는 진출 포인트라 차선 시작지점이 아님.

		// 짝수번째 차선 포인트 == 진입 포인트
		const FVector StartPoint = LanePoints[PointIdx];

		// 방향 별 진출 포인트. LanePoints는 4거리 각 지점을 시계 방향순으로 저장함.
		// 직진 == 시계 방향으로 3칸 전.
		const uint32  EndRightPointIdx = OmniMath::CircularNum(LanePoints.Num() - 1, PointIdx - 3);
		check(LanePoints.IsValidIndex(EndRightPointIdx))
		const FVector EndStraightPoint = LanePoints[EndRightPointIdx];
		const FVector LaneTangent      = EndStraightPoint - StartPoint;

		USplineComponent* const LaneSpline = GetLaneSpline(LaneIdx);
		if (OB_IS_VALID(LaneSpline) == false)
			continue;

		LaneSpline->SetLocationAtSplinePoint(0, StartPoint, CoordSpace);
		LaneSpline->SetLocationAtSplinePoint(1, EndStraightPoint, CoordSpace);
		LaneSpline->SetTangentAtSplinePoint(0, LaneTangent, CoordSpace);
		LaneSpline->SetTangentAtSplinePoint(1, LaneTangent, CoordSpace);
	}
}

void AOmniRoadIntersection4Way::SetCompTransform()
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	// for (int i = 0; i < Lane_ApproachBoxes.Num(); ++i)
	// {
	// 	UOmniLaneApproachCollision* const LaneApproach     = GetLaneApproachBox(i);
	// 	const USplineComponent* const     TargetLaneSpline = GetLaneSpline(i);
	// 	if (OB_IS_VALID(LaneApproach) == false || OB_IS_VALID(TargetLaneSpline) == false)
	// 		continue;
	//
	// 	LaneApproach->SetBoxExtent(BoxCollisionExtent);
	// 	const FVector    Lane_StartLoc       = TargetLaneSpline->GetLocationAtSplinePoint(0, CoordSpace);
	// 	const FVector    Lane_StartDirection = TargetLaneSpline->GetDirectionAtSplinePoint(0, CoordSpace);
	// 	const FTransform ApproachTransform   = OmniMath::GetTransformAddOffset(Lane_StartLoc, Lane_StartDirection, BoxCollisionOffset);
	//
	// 	LaneApproach->SetRelativeTransform(ApproachTransform);
	//
	// 	//디버그용 화살표
	// 	UArrowComponent* const LaneArrow = GetDebugLaneArrow(i);
	// 	if (OB_IS_VALID(LaneArrow) == false)
	// 		continue;
	//
	// 	LaneArrow->SetRelativeLocation(Lane_StartLoc);
	// 	LaneArrow->SetRelativeRotation(Lane_StartDirection.Rotation());
	// }
}

void AOmniRoadIntersection4Way::SetLanePoints()
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

USplineComponent* AOmniRoadIntersection4Way::GetLaneByApproachIdx(const uint32 ApproachIdx, const ERoadDirection& LaneIdx) const
{
	const uint32 LaneMaxIdx = LaneSplineNum / 4; // 12/4 = 3
	return GetLaneSpline(ApproachIdx * LaneMaxIdx + EnumToInt(LaneIdx));
}

USplineComponent* AOmniRoadIntersection4Way::GetLaneByApproachIdx(const uint32 ApproachIdx, const int& LaneIdx) const
{
	const uint32 LaneMaxIdx = LaneSplineNum / 4; // 12/4 = 3, 0~3
	return GetLaneSpline(ApproachIdx * LaneMaxIdx + LaneIdx);
}

USplineComponent* AOmniRoadIntersection4Way::GetLaneByApproachIdx(const ERoadApproach& ApproachIdx, const ERoadDirection& LaneIdx) const
{
	return GetLaneByApproachIdx(EnumToInt(ApproachIdx), EnumToInt(LaneIdx));
}

USplineComponent* AOmniRoadIntersection4Way::GetLeftTurnLane(const uint32 ApproachIdx) const
{
	return GetLaneByApproachIdx(ApproachIdx, ERoadDirection::Left);
}

USplineComponent* AOmniRoadIntersection4Way::GetStraightLane(const uint32 ApproachIdx) const
{
	return GetLaneByApproachIdx(ApproachIdx, ERoadDirection::Straight);
}

USplineComponent* AOmniRoadIntersection4Way::GetRightTurnLane(const uint32 ApproachIdx) const
{
	return GetLaneByApproachIdx(ApproachIdx, ERoadDirection::Right);
}

double AOmniRoadIntersection4Way::GetRoadWidth()
{
	if (OB_IS_VALID(Flat4WayMesh) == false || OB_IS_VALID(Flat4WayMesh->GetStaticMesh()) == false)
		return 10.0; // 오류 방지 기본 값.

	if (OB_IS_VALID(PlacedMesh) == false)
		PlacedMesh = Flat4WayMesh->GetStaticMesh();

	const FBox PlaceMeshBox = PlacedMesh->GetBoundingBox();
	return OmniMath::GetBoxWidth(PlaceMeshBox);
}

FIntersectionDimensionInfo AOmniRoadIntersection4Way::GetIntersectionDimensionInfo()
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
