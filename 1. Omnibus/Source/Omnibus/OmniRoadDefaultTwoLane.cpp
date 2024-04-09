// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
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

	InitArrays(2, 1, 2, 2);

	InitRoadSpline(0);

	// 에디터 작업 편의성을 위해 기본 스플라인 길이 및 탄젠트 설정. 가상함수라서 호출안하고 상수사용
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	const FVector           DefaultVector  = FVector(600, 0, 0);
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

	for (int i = 0; i < AccessPointNum; ++i)
	{
		InitLaneApproachBox(i, GetLaneSpline(i));
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
	SnapRoadSplineStartEndLocation();
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

void AOmniRoadDefaultTwoLane::SnapRoadSplineStartEndLocation()
{
	constexpr ESplineCoordinateSpace::Type SplineCoord_Local = ESplineCoordinateSpace::Local;

	// 액터와 각 스플라인 포인트 z축 0.0으로 고정.
	FVector TempActorLocation = GetActorLocation();
	if (TempActorLocation.Z != 0.0)
	{
		OB_LOG_STR("Note. Locking to z-axis 0.0")
		TempActorLocation.Z = 0.0;
	}
	
	for (int i = 0; i < GetMainRoadSpline()->GetNumberOfSplinePoints(); ++i)
	{
		FVector SplinePointLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i, SplineCoord_Local);
		if (SplinePointLocation.Z != 0.0)
		{
			SplinePointLocation.Z = 0.0;
			OB_LOG_STR("Note. Locking to z-axis 0.0")
			GetMainRoadSpline()->SetLocationAtSplinePoint(i, SplinePointLocation, SplineCoord_Local);
		}
	}

	SetActorLocation(OmniMath::RoundHalfToEvenVector(TempActorLocation, FOmniConst::Unit_Length));

	//스플라인 시작점 반올림.
	const FVector SplineStartPointPosition  = GetMainRoadSpline()->GetLocationAtSplinePoint(0, SplineCoord_Local);
	const FVector SplineStartPointRoundHalf = OmniMath::RoundHalfToEvenVector(SplineStartPointPosition, FOmniConst::Unit_Length);
	GetMainRoadSpline()->SetLocationAtSplinePoint(0, SplineStartPointRoundHalf, SplineCoord_Local);

	//스플라인 끝점 반올림.
	const FVector SplineLastPointPosition  = GetMainRoadSpline()->GetLocationAtSplinePoint(GetSplinePointLastIdx(), SplineCoord_Local);
	const FVector SplineLastPointRoundHalf = OmniMath::RoundHalfToEvenVector(SplineLastPointPosition, FOmniConst::Unit_Length);
	GetMainRoadSpline()->SetLocationAtSplinePoint(GetSplinePointLastIdx(), SplineLastPointRoundHalf, SplineCoord_Local);
}

void AOmniRoadDefaultTwoLane::MakeSplineMeshComponentAlongSpline()
{
	if (OB_IS_VALID(PlacedMesh) == false || OB_IS_VALID(GetMainRoadSpline()) == false)
		return;

	constexpr ESplineCoordinateSpace::Type SplineCoord_Local = ESplineCoordinateSpace::Local;

	//스플라인 컴포넌트 포인트를 따라 스플라인 메시 생성
	FVector StartLoc, StartTangent;
	FVector EndLoc, EndTangent;
	GetMainRoadSpline()->GetLocationAndTangentAtSplinePoint(0, StartLoc, StartTangent, SplineCoord_Local);

	const int32 SplinePointLastIdx = GetSplinePointLastIdx();
	for (int i = 0; i < SplinePointLastIdx; ++i)
	{
		GetMainRoadSpline()->GetLocationAndTangentAtSplinePoint(i + 1, EndLoc, EndTangent, SplineCoord_Local);

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

	constexpr ESplineCoordinateSpace::Type SplineCoord_Local = ESplineCoordinateSpace::Local;

	USplineComponent* LaneSpline_0 = GetLaneSpline(0);
	USplineComponent* LaneSpline_1 = GetLaneSpline(1);
	
	LaneSpline_0->ClearSplinePoints();
	LaneSpline_1->ClearSplinePoints();

	const int32 RoadSplinePointsNum = GetMainRoadSpline()->GetNumberOfSplinePoints();
	const double RoadWidth          = GetRoadWidth();

	// 메인 RoadSpline의 각 포인트에 맞춰, 좌우 차선 업데이트.
	for (int i = 0; i < RoadSplinePointsNum; ++i)
	{
		//각 포인트 위치 지정
		const FVector RoadSplineLocation    = GetMainRoadSpline()->GetLocationAtSplinePoint(i, SplineCoord_Local);
		const FVector RoadSplineRightVector = GetMainRoadSpline()->GetRightVectorAtSplinePoint(i, SplineCoord_Local);

		//각 스플라인 간격. 표현할 메시 너비의 1/4 지점에 각각 설치.
		const double SplineSpacing = RoadWidth * 0.25;

		const FVector Lane_01Location = RoadSplineLocation - RoadSplineRightVector * SplineSpacing;
		const FVector Lane_02Location = RoadSplineLocation + RoadSplineRightVector * SplineSpacing;

		LaneSpline_0->AddSplinePoint(Lane_01Location, SplineCoord_Local);
		LaneSpline_0->SetLocationAtSplinePoint(i, Lane_01Location, SplineCoord_Local);

		LaneSpline_1->AddSplinePoint(Lane_02Location, SplineCoord_Local);
		LaneSpline_1->SetLocationAtSplinePoint(i, Lane_02Location, SplineCoord_Local);

		//각 포인트 탄젠트 지정.
		const FVector RoadSplinePointTangent         = GetMainRoadSpline()->GetTangentAtSplinePoint(i, SplineCoord_Local);
		const double  InConnerSpacingByTangentRatio  = (RoadSplinePointTangent.Size() - (SplineSpacing * 2)) / RoadSplinePointTangent.Size();
		const double  OutConnerSpacingByTangentRatio = (RoadSplinePointTangent.Size() + (SplineSpacing * 2)) / RoadSplinePointTangent.Size();

		FVector RoadSplinePointTangent_01 = RoadSplinePointTangent;
		FVector RoadSplinePointTangent_02 = RoadSplinePointTangent;

		//다음 '중심' 포인트와 현재 '차선' 포인트를 비교해서 안쪽 차선 찾기
		FVector TargetPointToCompareLocation; //비교할 목표 지점
		if (i < RoadSplinePointsNum - 1)
			TargetPointToCompareLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i + 1, SplineCoord_Local);
		else
			TargetPointToCompareLocation = GetMainRoadSpline()->GetLocationAtSplinePoint(i - 1, SplineCoord_Local);

		const double Lane_01Distance = FVector::Distance(Lane_01Location, TargetPointToCompareLocation);
		const double Lane_02Distance = FVector::Distance(Lane_02Location, TargetPointToCompareLocation);

		//1번, 2번 중에 다음 중심 스플라인 포인트와 가까운 차선이 안쪽 차선
		if (Lane_01Distance < Lane_02Distance)
		{
			RoadSplinePointTangent_01 *= InConnerSpacingByTangentRatio;
			RoadSplinePointTangent_02 *= OutConnerSpacingByTangentRatio;
		}
		else if (Lane_01Distance > Lane_02Distance)
		{
			RoadSplinePointTangent_01 *= OutConnerSpacingByTangentRatio;
			RoadSplinePointTangent_02 *= InConnerSpacingByTangentRatio;
		}
		//else //같다면 직진. 중심선이랑 동일하게

		LaneSpline_0->SetTangentAtSplinePoint(i, RoadSplinePointTangent_01, SplineCoord_Local);
		LaneSpline_1->SetTangentAtSplinePoint(i, RoadSplinePointTangent_02, SplineCoord_Local);
	}

	// 1차선은 역방향이라 포인트 순서 뒤집어줌. 자동차 주행방향을 결정.
	TArray<FVector> Lane_01PointsLocation;
	TArray<FVector> Lane_01PointsTangent;
	for (int i = LaneSpline_0->GetNumberOfSplinePoints() - 1; i >= 0; --i)
	{
		Lane_01PointsLocation.Add(LaneSpline_0->GetLocationAtSplinePoint(i, SplineCoord_Local));
		Lane_01PointsTangent.Add(LaneSpline_0->GetTangentAtSplinePoint(i, SplineCoord_Local));
	}
	
	for (int i = 0; i < LaneSpline_0->GetNumberOfSplinePoints(); ++i)
	{
		LaneSpline_0->SetLocationAtSplinePoint(i, Lane_01PointsLocation[i], SplineCoord_Local);
		LaneSpline_0->SetTangentAtSplinePoint(i, Lane_01PointsTangent[i] * -1, SplineCoord_Local);
	}

	//차선 감지용 콜리전 설정.
	for (int idx = 0; idx < LaneSplines.Num(); ++idx)
	{
		const FVector Lane_StartLoc        = GetLaneSpline(idx)->GetLocationAtSplinePoint(0, SplineCoord_Local);
		const FVector Lane_StartDirection  = GetLaneSpline(idx)->GetDirectionAtSplinePoint(0, SplineCoord_Local);
		const FTransform ApproachTransform = OmniMath::GetTransformAddOffset(Lane_StartLoc, Lane_StartDirection, BoxCollisionOffset);

		GetLaneApproachBox(idx)->SetBoxExtent(BoxCollisionExtent);
		GetLaneApproachBox(idx)->SetRelativeTransform(ApproachTransform);

		//디버그용 화살 위치 지정.
		GetDebugLaneArrow(idx)->SetRelativeLocation(Lane_StartLoc);
		GetDebugLaneArrow(idx)->SetRelativeRotation(Lane_StartDirection.Rotation());
	}
}

USplineComponent* AOmniRoadDefaultTwoLane::GetNextSplinePath(const int32 InLaneApproachIdx, AOmniRoad* InNextTargetRoad)
{
	if (OB_IS_VALID(InNextTargetRoad))
	{
		const int NextRoadIdx = FindConnectedRoadIdx(InNextTargetRoad);
		if (NextRoadIdx != INDEX_NONE)
		{
			return GetLaneSpline(InLaneApproachIdx);
		}
	}

	return nullptr;
}

USplineComponent* AOmniRoadDefaultTwoLane::GetNextSplinePath(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad)
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
