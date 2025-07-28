// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniRoadTwoLane.h"

#include "OmnibusTypes.h"
#include "OmniConnectorComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "UtlLog.h"
#include "UTLStatics.h"

AOmniRoadTwoLane::AOmniRoadTwoLane()
{
	PrimaryActorTick.bCanEverTick = true;

	InitArrays(1, 2, 2);

	InitRoadSpline(0);

	// 에디터 작업 편의성을 위해 기본 스플라인 길이 및 탄젠트 설정. 가상함수라서 호출안하고 상수사용
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;
	const FVector DefaultVector = FVector(600, 0, 0);
	
	USplineComponent* const MainRoadSpline = GetRoadSpline(0);
	MainRoadSpline->SetLocationAtSplinePoint(0, FVector::Zero(), CoordSpace);
	MainRoadSpline->SetLocationAtSplinePoint(1, DefaultVector, CoordSpace);
	MainRoadSpline->SetTangentAtSplinePoint(0, DefaultVector, CoordSpace);
	MainRoadSpline->SetTangentAtSplinePoint(1, DefaultVector, CoordSpace);

	for (int i = 0; i < RoadConnectorNum; ++i)
	{
		InitRoadConnector(i, GetMainRoadSpline(), i);
	}

	for (int i = 0; i < LaneSplineNum; ++i)
	{
		InitLaneSpline(i, GetMainRoadSpline());
	}
}

#if WITH_EDITOR
void AOmniRoadTwoLane::PostEditMove(bool bFinished)
{
	SnapRoadSplineTerminalLocation();
	Super::PostEditMove(bFinished);
}
#endif // WITH_EDITOR

void AOmniRoadTwoLane::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniRoadTwoLane::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOmniRoadTwoLane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniRoadTwoLane::DetectAllConnectedOmniRoad()
{
	// 현재 도로의 감지용 컴포넌트 순회. 감지된 대상 도로를 찾음.
	for (UOmniConnectorComponent* const Connector : RoadConnectors)
	{
		if (UT_IS_VALID(Connector) == false)
			continue;

		// 스플라인 포인트 이동시, 감지용 컴포넌트도 같이 이동함.
		Connector->SetRelativeTransformToSpline();

		UOmniConnectorComponent* const DetectedTargetConnector = DetectOmniRoad(Connector);
		if(DetectedTargetConnector == nullptr)
			continue;
			
		Connector->ChangeSplineTangentNormal(DetectedTargetConnector);
	}
}

void AOmniRoadTwoLane::SnapRoadSplineTerminalLocation()
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	// 각 스플라인 포인트 z축 0.0으로 고정.
	for (int i = 0; i < GetMainRoadSpline()->GetNumberOfSplinePoints(); ++i)
	{
		FVector SplinePointLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i, CoordSpace);
		if (SplinePointLocation.Z != 0.0)
		{
			SplinePointLocation.Z = 0.0;
			GetMainRoadSpline()->SetLocationAtSplinePoint(i, SplinePointLocation, CoordSpace);
		}
	}
}

void AOmniRoadTwoLane::MakeSplineMeshComponentAlongSpline()
{
	if (UT_IS_VALID(PlacedMesh) == false || UT_IS_VALID(GetMainRoadSpline()) == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	//스플라인 컴포넌트 포인트를 따라 스플라인 메시 생성
	FVector StartLoc, StartTangent;
	FVector EndLoc, EndTangent;
	const USplineComponent* RoadSpline = GetMainRoadSpline();
	RoadSpline->GetLocationAndTangentAtSplinePoint(0, StartLoc, StartTangent, CoordSpace);

	const int32 SplinePointsSize = RoadSpline->GetNumberOfSplinePoints();
	for (int PointIdx = 1; PointIdx < SplinePointsSize; ++PointIdx)
	{
		RoadSpline->GetLocationAndTangentAtSplinePoint(PointIdx, EndLoc, EndTangent, CoordSpace);
		const float RoadSegmentLength = RoadSpline->GetDistanceAlongSplineAtSplinePoint(PointIdx) - RoadSpline->GetDistanceAlongSplineAtSplinePoint(PointIdx - 1);
		
		USplineMeshComponent* const NewSplineMesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), false, FTransform(), false));

		if (UT_IS_VALID(NewSplineMesh) == false)
			continue;

		// NewSplineMesh->SetStaticMesh(PlacedMesh);
		NewSplineMesh->SetStaticMesh(GetRoadMesh(RoadSegmentLength));
		NewSplineMesh->SetStartAndEnd(StartLoc, StartTangent, EndLoc, EndTangent);
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		NewSplineMesh->SetCanEverAffectNavigation(false);
		NewSplineMesh->SetCollisionProfileName(EOmniCollisionProfile::SysDetectorOverlap); // 버스 정류장 등 감지
		NewSplineMesh->SetMobility(EComponentMobility::Static);
		NewSplineMesh->SetCastShadow(false);

		StartLoc     = EndLoc;
		StartTangent = EndTangent;
	}
}

void AOmniRoadTwoLane::UpdateLaneSplinesAlongRoadCenter()
{
	if ( UT_IS_VALID(PlacedMesh) == false
		|| UT_IS_VALID(GetMainRoadSpline()) == false
		|| UT_IS_VALID(GetLaneSpline(0)) == false
		|| UT_IS_VALID(GetLaneSpline(1)) == false )
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	USplineComponent* const MainRoadSpline = GetMainRoadSpline();
	USplineComponent* const LaneSpline_0 = GetLaneSpline(0);
	USplineComponent* const LaneSpline_1 = GetLaneSpline(1);

	LaneSpline_0->ClearSplinePoints();
	LaneSpline_1->ClearSplinePoints();

#if WITH_EDITORONLY_DATA
	LaneSpline_0->bAllowDiscontinuousSpline = true; // 도착, 출발 탄젠트를 따로 제어
	LaneSpline_1->bAllowDiscontinuousSpline = true;
#endif // WITH_EDITORONLY_DATA
	
	const int32 RoadSplinePointsNum = MainRoadSpline->GetNumberOfSplinePoints();
	const double SplineSpacing      = GetRoadWidth() * 0.25; //각 스플라인 간격. 표현할 메시 너비의 1/4 지점에 각각 설치.

	// 메인 RoadSpline의 각 포인트에 맞춰, 좌우 차선 업데이트.
	for (int i = 0; i < RoadSplinePointsNum; ++i)
	{
		//각 포인트 위치 지정
		const FVector RoadSplineLocation    = MainRoadSpline->GetLocationAtSplinePoint(i, CoordSpace);
		const FVector RoadSplineRightVector = MainRoadSpline->GetRightVectorAtSplinePoint(i, CoordSpace);

		const FVector Lane_00Location = RoadSplineLocation - RoadSplineRightVector * SplineSpacing;
		const FVector Lane_01Location = RoadSplineLocation + RoadSplineRightVector * SplineSpacing;

		LaneSpline_0->AddSplinePoint(Lane_00Location, CoordSpace);
		LaneSpline_1->AddSplinePoint(Lane_01Location, CoordSpace);
		LaneSpline_0->SetSplinePointType(i, ConvertInterpCurveModeToSplinePointType(CIM_CurveUser));
		LaneSpline_1->SetSplinePointType(i, ConvertInterpCurveModeToSplinePointType(CIM_CurveUser));

		// 도로 탄젠트값과 안쪽, 바깥쪽 코너에 적용할 비율
		const FVector RoadPointArriveTangent     = MainRoadSpline->GetArriveTangentAtSplinePoint(i, CoordSpace);
		const FVector RoadPointLeaveTangent      = MainRoadSpline->GetLeaveTangentAtSplinePoint(i, CoordSpace);
		const double RoadPointArriveTangentSize  = RoadPointArriveTangent.Size();
		const double RoadPointLeaveTangentSize   = RoadPointLeaveTangent.Size();
		const double InConnerArriveTangentRatio  = (RoadPointArriveTangentSize - (SplineSpacing * 2)) / RoadPointArriveTangentSize;
		const double OutConnerArriveTangentRatio = (RoadPointArriveTangentSize + (SplineSpacing * 2)) / RoadPointArriveTangentSize;
		const double InConnerLeaveTangentRatio   = (RoadPointLeaveTangentSize - (SplineSpacing * 2)) / RoadPointLeaveTangentSize;
		const double OutConnerLeaveTangentRatio  = (RoadPointLeaveTangentSize + (SplineSpacing * 2)) / RoadPointLeaveTangentSize;

		FVector Lane_00ArriveTangent = RoadPointArriveTangent;
		FVector Lane_00LeaveTangent  = RoadPointLeaveTangent;
		FVector Lane_01ArriveTangent = RoadPointArriveTangent;
		FVector Lane_01LeaveTangent  = RoadPointLeaveTangent;

		// 이전,다음 도로 Point와 현재 양쪽 차선 Point의 거리를 측정. 더 가까운 차선이 안쪽 차선.
		// 범위 예외처리 안함. 자동 clamp됨. 넘어간 부분은 더이상 차선이 없기 때문에, 해당 방향의 탄젠트로 임의의 값이 들어가도됨.
		const FVector PrevPointLocation = MainRoadSpline->GetLocationAtSplinePoint(i - 1, CoordSpace);
		const FVector NextPointLocation = MainRoadSpline->GetLocationAtSplinePoint(i + 1, CoordSpace);

		const double Lane_00PrevDist = FVector::DistSquared(Lane_00Location, PrevPointLocation);
		const double Lane_00NextDist = FVector::DistSquared(Lane_00Location, NextPointLocation);
		const double Lane_01PrevDist = FVector::DistSquared(Lane_01Location, PrevPointLocation);
		const double Lane_01NextDist = FVector::DistSquared(Lane_01Location, NextPointLocation);

		// 이전 구간
		if (Lane_00PrevDist < Lane_01PrevDist)
		{
			Lane_00ArriveTangent *= InConnerArriveTangentRatio;
			Lane_01ArriveTangent *= OutConnerArriveTangentRatio;
		}
		else if(Lane_00PrevDist > Lane_01PrevDist)
		{
			Lane_00ArriveTangent *= OutConnerArriveTangentRatio;
			Lane_01ArriveTangent *= InConnerArriveTangentRatio;
		}
		
		// 다음 구간
		if (Lane_00NextDist < Lane_01NextDist)
		{
			Lane_00LeaveTangent *= InConnerLeaveTangentRatio;
			Lane_01LeaveTangent *= OutConnerLeaveTangentRatio;
		}
		else if (Lane_00NextDist > Lane_01NextDist)
		{
			Lane_00LeaveTangent *= OutConnerLeaveTangentRatio;
			Lane_01LeaveTangent *= InConnerLeaveTangentRatio;
		}
		// else // 거리가 같거나, 이전/다음 구간이 없다면 직진. 중심도로 값과 동일하게.

		LaneSpline_0->SetTangentsAtSplinePoint(i, Lane_00ArriveTangent, Lane_00LeaveTangent, CoordSpace);
		LaneSpline_1->SetTangentsAtSplinePoint(i, Lane_01ArriveTangent, Lane_01LeaveTangent, CoordSpace);
	}

	// 1차선은 역방향이라 포인트 순서 뒤집어줌. 자동차 주행방향을 결정.
	TArray<FVector> Lane_00PointsLocation;
	TArray<FVector> Lane_00PointsArriveTangent;
	TArray<FVector> Lane_00PointsLeaveTangent;
	for (int i = LaneSpline_0->GetNumberOfSplinePoints() - 1; i >= 0; --i)
	{
		Lane_00PointsLocation.Add(LaneSpline_0->GetLocationAtSplinePoint(i, CoordSpace));
		Lane_00PointsArriveTangent.Add(LaneSpline_0->GetArriveTangentAtSplinePoint(i, CoordSpace));
		Lane_00PointsLeaveTangent.Add(LaneSpline_0->GetLeaveTangentAtSplinePoint(i, CoordSpace));
	}

	for (int i = 0; i < LaneSpline_0->GetNumberOfSplinePoints(); ++i)
	{
		LaneSpline_0->SetLocationAtSplinePoint(i, Lane_00PointsLocation[i], CoordSpace);
		LaneSpline_0->SetTangentsAtSplinePoint(i, Lane_00PointsLeaveTangent[i] * -1, Lane_00PointsArriveTangent[i] * -1, CoordSpace);
	}
	LaneSpline_0->UpdateSpline();
	LaneSpline_1->UpdateSpline();
}

AOmniRoad* AOmniRoadTwoLane::GetNextRoadByLaneIdx(const int32 InLaneIdx)
{
	// 2차선은 이웃 도로의 Idx와 해당 도로로 가는 차선의 번호가 동일.
	return GetConnectedRoad(InLaneIdx);
}

int32 AOmniRoadTwoLane::FindLaneIdxToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	// 같은 경우, 잘못된 입력.
	// 시작 지점의 경우, 이전 도로에 nullptr가 오기 때문에 체크 안함.
	if ((InPrevRoad != InNextTargetRoad) && UT_IS_VALID(InNextTargetRoad))
	{
		// 2차선은 이웃 도로의 Idx와 해당 도로로 가는 차선의 번호가 동일.
		return FindConnectedRoadIdx(InNextTargetRoad);
	}
	return INDEX_NONE;
}

USplineComponent* AOmniRoadTwoLane::GetMainRoadSpline() const
{
	return GetRoadSpline(0);
}

UStaticMesh* AOmniRoadTwoLane::GetRoadMesh(const float InLaneLength)
{
	// 없는 경우 임시 조치
	UT_IF(LaneMeshList.IsEmpty())
		return PlacedMesh;

	for (const FRoadLengthMesh& RoadLengthMesh : LaneMeshList)
	{
		if (RoadLengthMesh.RoadLength < InLaneLength)
			return RoadLengthMesh.RoadMesh;
	}

	return PlacedMesh;
}
