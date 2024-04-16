// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniDetectSphereComponent.h"

#include "OmnibusUtilities.h"
#include "OmniRoad.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UOmniDetectSphereComponent::UOmniDetectSphereComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerOmniRoad            = TWeakObjectPtr<AOmniRoad>(nullptr);
	OwnerSpline              = TWeakObjectPtr<USplineComponent>(nullptr);
	SplinePointPosition      = ERoadConnectorPoint::None;
	DetectedTargetOmniRoadID = 0;
	AccessPointIdx           = 0;
}

void UOmniDetectSphereComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOmniDetectSphereComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UOmniDetectSphereComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOmniDetectSphereComponent::SetOwnerOmniRoadAndSpline(AOmniRoad* InOwnerOmniRoad, USplineComponent* InOwnerSpline, const ERoadConnectorPoint InSplinePointPosition, const uint8 InAccessPointIdx)
{
	OwnerOmniRoad       = OB_IS_VALID(InOwnerOmniRoad) ? InOwnerOmniRoad : nullptr;
	OwnerSpline         = OB_IS_VALID(InOwnerSpline) ? InOwnerSpline : nullptr;
	SplinePointPosition = InSplinePointPosition;
	AccessPointIdx      = InAccessPointIdx;
}

void UOmniDetectSphereComponent::SetOwnerOmniRoadAndSpline(AOmniRoad*  InOwnerOmniRoad, USplineComponent* InOwnerSpline, const uint32 InSplinePointIdx, const uint8 InAccessPointIdx)
{
	SetOwnerOmniRoadAndSpline(InOwnerOmniRoad, InOwnerSpline
	                        , (InSplinePointIdx == 0) ? ERoadConnectorPoint::Start : ERoadConnectorPoint::End
	                        , InAccessPointIdx);
}

AOmniRoad* UOmniDetectSphereComponent::GetOwnerOmniRoad() const
{
	if (OwnerOmniRoad.IsValid())
		return OwnerOmniRoad.Get();

	return nullptr;
}

int UOmniDetectSphereComponent::GetSplinePointIdx() const
{
	int PointIdx(0);

	if (SplinePointPosition == ERoadConnectorPoint::Start)
		PointIdx = 0;
	else
		PointIdx = OwnerSpline->GetNumberOfSplinePoints() - 1;

	return PointIdx;
}

ERoadConnectorPoint UOmniDetectSphereComponent::GetSplinePointPosition() const
{
	return SplinePointPosition;
}

void UOmniDetectSphereComponent::SetRelativeTransformToSpline()
{
	if (OwnerSpline.IsValid() == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	SetRelativeLocation(OwnerSpline->GetLocationAtSplinePoint(GetSplinePointIdx(), CoordSpace));
	SetRelativeRotation(OwnerSpline->GetDirectionAtSplinePoint(GetSplinePointIdx(), CoordSpace).Rotation());
}

FVector UOmniDetectSphereComponent::GetSplinePointInsideTangent()
{
	if (OB_IS_WEAK_PTR_VALID(OwnerSpline) == false)
		return FVector::ZeroVector;
	
	constexpr ESplineCoordinateSpace::Type TangentSplineCoord = ESplineCoordinateSpace::World;
	FVector OutTangent = FVector::ZeroVector;

	switch (EnumToInt(SplinePointPosition))
	{
		case EnumToInt(ERoadConnectorPoint::Start):
			OutTangent = OwnerSpline->GetTangentAtSplinePoint(GetSplinePointIdx(), TangentSplineCoord);
			break;
		case EnumToInt(ERoadConnectorPoint::End):
			OutTangent = OwnerSpline->GetTangentAtSplinePoint(GetSplinePointIdx(), TangentSplineCoord) * -1.0;
			break;
		default:
			break;
	}
	return OutTangent;
}

FVector UOmniDetectSphereComponent::GetSplinePointInsideTangentNormal()
{
	return GetSplinePointInsideTangent().GetSafeNormal();
}

void UOmniDetectSphereComponent::ChangeSplineTangentNormal(UOmniDetectSphereComponent* InTarget) const
{
	if (OB_IS_WEAK_PTR_VALID(OwnerSpline) == false)
		return;

	constexpr ESplineCoordinateSpace::Type TangentSplineCoord = ESplineCoordinateSpace::World;

	// 연결된 도로의 탄젠트 노말로 자신의 탄젠트 수정.
	const FVector MyTangent      = OwnerSpline->GetTangentAtSplinePoint(GetSplinePointIdx(), TangentSplineCoord);
	const FVector ChangedTangent = (InTarget->GetSplinePointInsideTangentNormal() * MyTangent.Size()) * GetStartPointTangentFactor();

	OwnerSpline->SetTangentAtSplinePoint(GetSplinePointIdx(), ChangedTangent, TangentSplineCoord);
}

double UOmniDetectSphereComponent::GetStartPointTangentFactor() const
{
	// 상대 탄젠트는 모두 안쪽을 바라보기 때문에, 반대방향을 바라보는 시작 지점의 탄젠트에 적용하기 위해 -1을 곱해줌.
	if (GetSplinePointPosition() == ERoadConnectorPoint::Start)
		return -1.0;

	return 1.0;
}

TArray<UPrimitiveComponent*> UOmniDetectSphereComponent::GetOverlapComps(UClass* TargetComponentClassFilter)
{
	const EObjectTypeQuery       CollObjectType = UEngineTypes::ConvertToObjectType(GetCollisionObjectType());
	TArray<UPrimitiveComponent*> OverlappingComps;

	UKismetSystemLibrary::ComponentOverlapComponents(this
	                                               , GetComponentTransform()
	                                               , {CollObjectType}
	                                               , TargetComponentClassFilter
	                                               , TArray<AActor*>()
	                                               , OverlappingComps);

	return OverlappingComps;
}
