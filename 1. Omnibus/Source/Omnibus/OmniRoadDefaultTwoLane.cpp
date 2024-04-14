// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniRoadDefaultTwoLane.h"

#include "Omnibus.h"
#include "OmnibusGameInstance.h"
#include "OmnibusRoadManager.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniDetectSphereComponent.h"
#include "Components/ArrowComponent.h"
#include "OmniLaneApproachCollision.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

AOmniRoadDefaultTwoLane::AOmniRoadDefaultTwoLane()
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

	for (int i = 0; i < RoadConnectDetectorNum; ++i)
	{
		InitRoadConnectDetector(i, GetMainRoadSpline(), i);
	}

	for (int i = 0; i < LaneSplineNum; ++i)
	{
		InitLaneSpline(i, GetMainRoadSpline());
	}
}

void AOmniRoadDefaultTwoLane::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniRoadDefaultTwoLane::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Cast<UOmnibusGameInstance>(GetGameInstance())->GetOmnibusRoadManager()->RemoveOmniRoad(GetOmniID());
}

void AOmniRoadDefaultTwoLane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniRoadDefaultTwoLane::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	SnapRoadSplineTerminalLocation();
}

void AOmniRoadDefaultTwoLane::DetectAllConnectedOmniRoad()
{
	// 현재 도로의 감지용 컴포넌트 순회. 감지된 대상 도로를 찾음.
	for (UOmniDetectSphereComponent* const Detector : RoadConnectDetectors)
	{
		if (OB_IS_VALID(Detector) == false)
			continue;

		// 스플라인 포인트 이동시, 감지용 컴포넌트도 같이 이동함.
		Detector->SetRelativeTransformToSpline();

		UOmniDetectSphereComponent* const DetectedTargetSphere = DetectOmniRoad(Detector);
		if(DetectedTargetSphere == nullptr)
			continue;
			
		Detector->ChangeSplineTangentNormal(DetectedTargetSphere);
	}
}

void AOmniRoadDefaultTwoLane::SnapRoadSplineTerminalLocation()
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	// 각 스플라인 포인트 z축 0.0으로 고정.
	for (int i = 0; i < GetMainRoadSpline()->GetNumberOfSplinePoints(); ++i)
	{
		FVector SplinePointLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i, CoordSpace);
		if (SplinePointLocation.Z != 0.0)
		{
			SplinePointLocation.Z = 0.0;
			OB_LOG_STR("Note. Locking to z-axis 0.0")
			GetMainRoadSpline()->SetLocationAtSplinePoint(i, SplinePointLocation, CoordSpace);
		}
	}

	//스플라인 시작점 반올림.
	const FVector SplineStartPointPosition  = GetMainRoadSpline()->GetLocationAtSplinePoint(0, CoordSpace);
	const FVector SplineStartPointRoundHalf = OmniMath::RoundHalfToEvenVector(SplineStartPointPosition, FOmniConst::Unit_Length);
	GetMainRoadSpline()->SetLocationAtSplinePoint(0, SplineStartPointRoundHalf, CoordSpace);

	//스플라인 끝점 반올림.
	const FVector SplineLastPointPosition  = GetMainRoadSpline()->GetLocationAtSplinePoint(GetSplinePointLastIdx(), CoordSpace);
	const FVector SplineLastPointRoundHalf = OmniMath::RoundHalfToEvenVector(SplineLastPointPosition, FOmniConst::Unit_Length);
	GetMainRoadSpline()->SetLocationAtSplinePoint(GetSplinePointLastIdx(), SplineLastPointRoundHalf, CoordSpace);
}

void AOmniRoadDefaultTwoLane::MakeSplineMeshComponentAlongSpline()
{
	if (OB_IS_VALID(PlacedMesh) == false || OB_IS_VALID(GetMainRoadSpline()) == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	//스플라인 컴포넌트 포인트를 따라 스플라인 메시 생성
	FVector StartLoc, StartTangent;
	FVector EndLoc, EndTangent;
	GetMainRoadSpline()->GetLocationAndTangentAtSplinePoint(0, StartLoc, StartTangent, CoordSpace);

	const int32 SplinePointLastIdx = GetSplinePointLastIdx();
	for (int i = 0; i < SplinePointLastIdx; ++i)
	{
		GetMainRoadSpline()->GetLocationAndTangentAtSplinePoint(i + 1, EndLoc, EndTangent, CoordSpace);

		USplineMeshComponent* const NewSplineMesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), false, FTransform(), false));

		if (OB_IS_VALID(NewSplineMesh) == false)
			continue;

		NewSplineMesh->SetStaticMesh(PlacedMesh);
		NewSplineMesh->SetStartAndEnd(StartLoc, StartTangent, EndLoc, EndTangent);
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		NewSplineMesh->SetCanEverAffectNavigation(true);
		NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		NewSplineMesh->SetCollisionProfileName(FName("BlockAllDynamic"));

		StartLoc     = EndLoc;
		StartTangent = EndTangent;
	}
}

void AOmniRoadDefaultTwoLane::UpdateLaneSplinesAlongRoadCenter()
{
	if ( OB_IS_VALID(PlacedMesh) == false
		|| OB_IS_VALID(GetMainRoadSpline()) == false
		|| OB_IS_VALID(GetLaneSpline(0)) == false
		|| OB_IS_VALID(GetLaneSpline(1)) == false )
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	USplineComponent* const LaneSpline_0 = GetLaneSpline(0);
	USplineComponent* const LaneSpline_1 = GetLaneSpline(1);

	LaneSpline_0->ClearSplinePoints();
	LaneSpline_1->ClearSplinePoints();
	LaneSpline_0->bAllowDiscontinuousSpline = true; // 도착, 출발 탄젠트를 따로 제어
	LaneSpline_1->bAllowDiscontinuousSpline = true;

	const int32 RoadSplinePointsNum = GetMainRoadSpline()->GetNumberOfSplinePoints();
	const double SplineSpacing      = GetRoadWidth() * 0.25; //각 스플라인 간격. 표현할 메시 너비의 1/4 지점에 각각 설치.

	// 메인 RoadSpline의 각 포인트에 맞춰, 좌우 차선 업데이트.
	for (int i = 0; i < RoadSplinePointsNum; ++i)
	{
		//각 포인트 위치 지정
		const FVector RoadSplineLocation    = GetMainRoadSpline()->GetLocationAtSplinePoint(i, CoordSpace);
		const FVector RoadSplineRightVector = GetMainRoadSpline()->GetRightVectorAtSplinePoint(i, CoordSpace);

		const FVector Lane_00Location = RoadSplineLocation - RoadSplineRightVector * SplineSpacing;
		const FVector Lane_01Location = RoadSplineLocation + RoadSplineRightVector * SplineSpacing;

		LaneSpline_0->AddSplinePoint(Lane_00Location, CoordSpace);
		LaneSpline_1->AddSplinePoint(Lane_01Location, CoordSpace);
		LaneSpline_0->SetSplinePointType(i, ConvertInterpCurveModeToSplinePointType(CIM_CurveUser));
		LaneSpline_1->SetSplinePointType(i, ConvertInterpCurveModeToSplinePointType(CIM_CurveUser));

		const FInterpCurvePointVector& RoadPointCurve = GetMainRoadSpline()->SplineCurves.Position.Points[i];

		// 도로 탄젠트값과 안쪽, 바깥쪽 코너에 적용할 비율
		const FVector RoadPointArriveTangent     = RoadPointCurve.ArriveTangent;
		const FVector RoadPointLeaveTangent      = RoadPointCurve.LeaveTangent;
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
		const FVector PrevPointLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i - 1, CoordSpace);
		const FVector NextPointLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i + 1, CoordSpace);

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

		FInterpCurvePointVector& Lane_00PointCurve = LaneSpline_0->SplineCurves.Position.Points.Last();
		FInterpCurvePointVector& Lane_01PointCurve = LaneSpline_1->SplineCurves.Position.Points.Last();

		Lane_00PointCurve.ArriveTangent = Lane_00ArriveTangent;
		Lane_00PointCurve.LeaveTangent  = Lane_00LeaveTangent;

		Lane_01PointCurve.ArriveTangent = Lane_01ArriveTangent;
		Lane_01PointCurve.LeaveTangent  = Lane_01LeaveTangent;
	}

	// 1차선은 역방향이라 포인트 순서 뒤집어줌. 자동차 주행방향을 결정.
	TArray<FVector> Lane_00PointsLocation;
	TArray<FVector> Lane_00PointsArriveTangent;
	TArray<FVector> Lane_00PointsLeaveTangent;
	for (int i = LaneSpline_0->GetNumberOfSplinePoints() - 1; i >= 0; --i)
	{
		Lane_00PointsLocation.Add(LaneSpline_0->GetLocationAtSplinePoint(i, CoordSpace));
		Lane_00PointsArriveTangent.Add(LaneSpline_0->SplineCurves.Position.Points[i].ArriveTangent);
		Lane_00PointsLeaveTangent.Add(LaneSpline_0->SplineCurves.Position.Points[i].LeaveTangent);
	}

	for (int i = 0; i < LaneSpline_0->GetNumberOfSplinePoints(); ++i)
	{
		LaneSpline_0->SetLocationAtSplinePoint(i, Lane_00PointsLocation[i], CoordSpace);
		LaneSpline_0->SplineCurves.Position.Points[i].LeaveTangent  = Lane_00PointsArriveTangent[i] * -1;
		LaneSpline_0->SplineCurves.Position.Points[i].ArriveTangent = Lane_00PointsLeaveTangent[i] * -1;
	}
	LaneSpline_0->UpdateSpline();
	LaneSpline_1->UpdateSpline();

	for (int idx = 0; idx < LaneSplines.Num(); ++idx)
	{
		const FVector Lane_StartLoc        = GetLaneSpline(idx)->GetLocationAtSplinePoint(0, CoordSpace);
		const FVector Lane_StartDirection  = GetLaneSpline(idx)->GetDirectionAtSplinePoint(0, CoordSpace);

		//디버그용 화살 위치 지정.
		GetDebugLaneArrow(idx)->SetRelativeLocation(Lane_StartLoc);
		GetDebugLaneArrow(idx)->SetRelativeRotation(Lane_StartDirection.Rotation());
	}
}

USplineComponent* AOmniRoadDefaultTwoLane::GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
{
	if (OB_IS_VALID(InPrevRoad) && OB_IS_VALID(InNextTargetRoad))
	{
		// 2차선은 접근 위치 번호와 진행방향이 반대
		const int NextvRoadIdx = FindConnectedRoadIdx(InNextTargetRoad);
		
		if (NextvRoadIdx != INDEX_NONE)
		{
			return GetLaneSpline(NextvRoadIdx);
		}
	}

	return nullptr;
}

USplineComponent* AOmniRoadDefaultTwoLane::GetMainRoadSpline() const
{
	return GetRoadSpline(0);
}

int AOmniRoadDefaultTwoLane::GetSplinePointLastIdx()
{
	return GetMainRoadSpline()->GetNumberOfSplinePoints() - 1;
}
