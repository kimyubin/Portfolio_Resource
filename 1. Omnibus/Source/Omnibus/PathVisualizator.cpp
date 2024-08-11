// Fill out your copyright notice in the Description page of Project Settings.


#include "PathVisualizator.h"

#include "OmnibusPlayData.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniLineBusRoute.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"


APathVisualizator::APathVisualizator()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static);

	NewSpline = CreateDefaultSubobject<USplineComponent>(TEXT("NewSpline"));
	NewSpline->SetupAttachment(RootComponent);

	PathColor  = FLinearColor::Black;
	PathOffset = FVector(0.0, 0.0, 30.0);
}

int32 APathVisualizator::GetLastPointIdx() const
{
	return NewSpline->GetNumberOfSplinePoints() - 1;
}

void APathVisualizator::MakeSplineMesh(const FVector& InStartLoc, const FVector& InStartTangent, const FVector& InEndLoc, const FVector& InEndTangent)
{
	constexpr bool bManualAttachment = false;
	USplineMeshComponent* const NewSplineMesh
			= Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), bManualAttachment, FTransform(), true));

	if (OB_IS_VALID(NewSplineMesh) == false)
		return;

	NewSplineMesh->SetStaticMesh(GetOmnibusPlayData()->GetPathVisualMesh());
	NewSplineMesh->SetStartAndEnd(InStartLoc + PathOffset, InStartTangent, InEndLoc + PathOffset, InEndTangent);
	NewSplineMesh->SetVisibility(true);
	NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
	NewSplineMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	NewSplineMesh->SetHiddenInGame(false);
	NewSplineMesh->SetGenerateOverlapEvents(false);
	NewSplineMesh->SetCastShadow(false);

	UMaterialInstanceDynamic* NewMatInstanceDynamic = NewSplineMesh->CreateDynamicMaterialInstance(0, NewSplineMesh->GetMaterial(0));
	if (IsValid(NewMatInstanceDynamic))
	{
		NewMatInstanceDynamic->SetVectorParameterValue(TEXT("Color"), PathColor);
		NewSplineMesh->SetMaterial(0, NewMatInstanceDynamic);
	}

	FinishAddComponent(NewSplineMesh, bManualAttachment, FTransform());
}

void APathVisualizator::VisualizeTransferRoutes(const FTransferPath& InTransferPath, const FLinearColor& InPathColor, const FVector& InPathOffset)
{
	MyTransferPath = InTransferPath;
	PathColor      = InPathColor;
	PathOffset     = InPathOffset;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	// 기존 메시 제거.
	TInlineComponentArray<USplineMeshComponent*> SplineMeshes;
	GetComponents(USplineMeshComponent::StaticClass(), SplineMeshes);
	for (USplineMeshComponent* SplineMesh : SplineMeshes)
	{
		SplineMesh->DestroyComponent();
	}

	// 스플라인 초기화
	const int32 DefaultSplineNum = NewSpline->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < DefaultSplineNum; ++PointIdx)
	{
		NewSpline->RemoveSplinePoint(0);
	}
	NewSpline->AddSplinePoint(FVector::Zero(), CoordSpace);


#if WITH_EDITORONLY_DATA
	NewSpline->bAllowDiscontinuousSpline = true;
#endif // WITH_EDITORONLY_DATA

	// 각 스탭은 버스 노선의 일부분임.
	for (const FTransferStep& CurrentStep : MyTransferPath.TransferStepList)
	{
		const AOmniLineBusRoute* BusLine = CurrentStep.BusLine.Get();

		// 마지막 정류장
		if (BusLine == nullptr)
			break;

		const USplineComponent* LineSpline = BusLine->GetRouteSpline();
		const float StartStopDistance      = BusLine->GetBusStopDist(CurrentStep.StartBusStopDistanceIdx).Distance;
		const float EndStopDistance        = BusLine->GetBusStopDist(CurrentStep.EndBusStopDistanceIdx).Distance;
		const float StartInputKey          = LineSpline->GetInputKeyValueAtDistanceAlongSpline(StartStopDistance);
		const float EndInputKey            = LineSpline->GetInputKeyValueAtDistanceAlongSpline(EndStopDistance);
		const int32 StartNextPointIndex    = static_cast<int32>(ceil(StartInputKey));
		const int32 EndPrevPointIndex      = static_cast<int32>(floor(EndInputKey));

		FVector StartLoc     = LineSpline->GetLocationAtDistanceAlongSpline(StartStopDistance, CoordSpace);
		FVector StartTangent = LineSpline->GetTangentAtDistanceAlongSpline(StartStopDistance, CoordSpace);
		FVector EndLoc       = LineSpline->GetLocationAtSplinePoint(StartNextPointIndex, CoordSpace);
		FVector EndTangent   = LineSpline->GetArriveTangentAtSplinePoint(StartNextPointIndex, CoordSpace);

		// 감소 비율 적용
		const float PrevFirstStopDist  = LineSpline->GetDistanceAlongSplineAtSplinePoint(StartNextPointIndex - 1);
		const float NextFirstStopDist  = LineSpline->GetDistanceAlongSplineAtSplinePoint(StartNextPointIndex);
		const float BeginReductionRate = (NextFirstStopDist - StartStopDistance) / (NextFirstStopDist - PrevFirstStopDist);

		StartTangent *= BeginReductionRate;
		EndTangent *= BeginReductionRate;

		MakeSplineMesh(StartLoc, StartTangent, EndLoc, EndTangent);
		{
			NewSpline->AddSplinePoint(FVector::Zero(), CoordSpace);

			NewSpline->SetLocationAtSplinePoint(GetLastPointIdx() - 1, StartLoc, CoordSpace);
			NewSpline->SetTangentsAtSplinePoint(GetLastPointIdx() - 1, FVector::Zero(), StartTangent, CoordSpace);
			NewSpline->SetLocationAtSplinePoint(GetLastPointIdx(), EndLoc, CoordSpace);
			NewSpline->SetTangentsAtSplinePoint(GetLastPointIdx(), EndTangent, FVector::Zero(), CoordSpace);
			// SimpleTextRender(StartLoc+FVector(0,0,100), 0, ("StartStart"), 200)
		}

		// EndPrevPointIndex 범위 미포함. NextIdx에서 추가됨.
		const int32 LineSplinePointNum = LineSpline->GetNumberOfSplinePoints();
		For_CircularRange(PointIdx, StartNextPointIndex, EndPrevPointIndex, LineSplinePointNum)
		{
			const int NextIdx = OmniMath::CircularNum(LineSplinePointNum - 1, PointIdx + 1);
			StartLoc          = LineSpline->GetLocationAtSplinePoint(PointIdx, CoordSpace);
			StartTangent      = LineSpline->GetLeaveTangentAtSplinePoint(PointIdx, CoordSpace);
			EndLoc            = LineSpline->GetLocationAtSplinePoint(NextIdx, CoordSpace);
			EndTangent        = LineSpline->GetArriveTangentAtSplinePoint(NextIdx, CoordSpace);

			MakeSplineMesh(StartLoc, StartTangent, EndLoc, EndTangent);
			{
				NewSpline->AddSplinePoint(FVector::Zero(), CoordSpace);

				NewSpline->SetTangentsAtSplinePoint(GetLastPointIdx() - 1, NewSpline->GetArriveTangentAtSplinePoint(GetLastPointIdx() - 1, CoordSpace), StartTangent, CoordSpace);
				NewSpline->SetLocationAtSplinePoint(GetLastPointIdx(), EndLoc, CoordSpace);
				NewSpline->SetTangentsAtSplinePoint(GetLastPointIdx(), EndTangent, NewSpline->GetLeaveTangentAtSplinePoint(GetLastPointIdx(), CoordSpace), CoordSpace);
			}
		}

		StartLoc     = LineSpline->GetLocationAtSplinePoint(EndPrevPointIndex, CoordSpace);
		StartTangent = LineSpline->GetLeaveTangentAtSplinePoint(EndPrevPointIndex, CoordSpace);
		EndLoc       = LineSpline->GetLocationAtDistanceAlongSpline(EndStopDistance, CoordSpace);
		EndTangent   = LineSpline->GetTangentAtDistanceAlongSpline(EndStopDistance, CoordSpace);

		const float PrevLastStopDist = LineSpline->GetDistanceAlongSplineAtSplinePoint(EndPrevPointIndex);
		const float NextLastStopDist = LineSpline->GetDistanceAlongSplineAtSplinePoint(EndPrevPointIndex + 1);
		const float EndReductionRate = (EndStopDistance - PrevLastStopDist) / (NextLastStopDist - PrevLastStopDist);

		StartTangent *= EndReductionRate;
		EndTangent *= EndReductionRate;

		MakeSplineMesh(StartLoc, StartTangent, EndLoc, EndTangent);
		{
			NewSpline->AddSplinePoint(FVector::Zero(), CoordSpace);

			NewSpline->SetTangentsAtSplinePoint(GetLastPointIdx() - 1, NewSpline->GetArriveTangentAtSplinePoint(GetLastPointIdx() - 1, CoordSpace), StartTangent, CoordSpace);
			NewSpline->SetLocationAtSplinePoint(GetLastPointIdx(), EndLoc, CoordSpace);
			NewSpline->SetTangentsAtSplinePoint(GetLastPointIdx(), EndTangent, FVector::Zero(), CoordSpace);
			// SimpleTextRender(EndLoc+FVector(0,0,100), 0, ("EndEnd"), 200)
		}
	}
}

void APathVisualizator::SetPathVisibility(const bool SetVisibility) const
{
	RootComponent->SetVisibility(SetVisibility, true);
}

bool APathVisualizator::IsSamePath(const FTransferPath& InOtherPath) const
{
	return MyTransferPath.IsValidPath() && MyTransferPath == InOtherPath;
}
