// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniCityBlock.h"

#include "Omnibus.h"
#include "OmnibusRoadManager.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniCityBlockWidget.h"
#include "OmniRoad.h"
#include "OmniStationBusStop.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/WidgetComponent.h"

AOmniCityBlock::AOmniCityBlock()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BlockOutLine = CreateDefaultSubobject<USplineComponent>(TEXT("BlockOutLine"));
	BlockOutLine->SetClosedLoop(true);
	BlockOutLine->SetupAttachment(RootComponent);
#if WITH_EDITORONLY_DATA
	BlockOutLine->bAllowDiscontinuousSpline = true; // 도착, 출발 탄젠트를 따로 제어
#endif // WITH_EDITORONLY_DATA

	// 기본 모양
	constexpr double Width = 1000.0;
	const TArray<FVector> Points = {FVector::ZeroVector, FVector(Width, 0.0, 0.0), FVector(Width, Width, 0.0), FVector(0.0, Width, 0.0)};
	BlockOutLine->SetSplinePoints(Points, ESplineCoordinateSpace::Local);
	for (int PointIdx = 0; PointIdx < BlockOutLine->GetNumberOfSplinePoints(); ++PointIdx)
	{
		BlockOutLine->SetTangentAtSplinePoint(PointIdx, FVector::ZeroVector, ESplineCoordinateSpace::Local);
	}
	
	TypeSignWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TypeSignWidgetComponent"));
	TypeSignWidgetComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TypeSignWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UOmniCityBlockWidget> TypeSignWidgetUI(TEXT("/Game/UIs/BP_CityBlockTypeSignWidget"));
	if (TypeSignWidgetUI.Succeeded())
		TypeSignWidgetComponent->SetWidgetClass(TypeSignWidgetUI.Class);

	CurrentType = ECityBlockType::None;
}

void AOmniCityBlock::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
}

#if WITH_EDITOR
void AOmniCityBlock::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	SetZ_Axis();
}
#endif // WITH_EDITOR

void AOmniCityBlock::BeginPlay()
{
	Super::BeginPlay();

	CurrentType = static_cast<ECityBlockType>(OmniMath::GetIntRandom(static_cast<int>(ECityBlockType::None), static_cast<int>(ECityBlockType::Size) - 1));
		                                                              
	UOmniCityBlockWidget* TypeSignWidget = Cast<UOmniCityBlockWidget>(TypeSignWidgetComponent->GetUserWidgetObject());
	if (TypeSignWidget)
	{
		TypeSignWidget->SetOwnerCityBlock(this);
	}
}

void AOmniCityBlock::PostBeginPlay()
{
	Super::PostBeginPlay();

	// 섹터의 위치는 임시로 버스 정류장의 위치로 지정
	SubSectorList.Reset();
	for (TWeakObjectPtr<AOmniStationBusStop>& StopWeak : OwnedBusStops)
	{
		const AOmniStationBusStop* Stop = StopWeak.Get();
		if (Stop == nullptr)
			continue;

		// 임시 오프셋
		constexpr double Offset = 500.0;

		const float ClosestInputKey    = BlockOutLine->FindInputKeyClosestToWorldLocation(Stop->GetActorLocation());
		const FVector ClosestLocation  = BlockOutLine->GetLocationAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::Local);
		const FVector Inside           = BlockOutLine->GetRightVectorAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::Local);
		const FVector RelativeLocation = ClosestLocation + (Inside * Offset);

		SubSectorList.Emplace(this, StopWeak, RelativeLocation);
		// SimpleTextRender(RelativeLocation+GetActorLocation(), 0, ("0"), 150);
	}
}

void AOmniCityBlock::MakeCollisionAlongSpline()
{
	if (OB_IS_VALID(OutLineMesh) == false)
		return;

	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local;

	GeneratedMeshes.Reset(BlockOutLine->GetNumberOfSplinePoints());
	
	// 스플라인 컴포넌트 포인트를 따라 스플라인 메시 생성
	const int32 SplinePointsSize = BlockOutLine->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < SplinePointsSize; ++PointIdx)
	{
		// OmniMath::CircularNum(SplinePointsSize - 1, PointIdx);
		// 마지막 포인트와 처음 포인트 연결.
		const int NextIdx    = PointIdx + 1 < SplinePointsSize ? PointIdx + 1 : 0;
		const FVector StartLoc     = BlockOutLine->GetLocationAtSplinePoint(PointIdx, CoordSpace);
		const FVector StartTangent = BlockOutLine->GetLeaveTangentAtSplinePoint(PointIdx, CoordSpace); // 출발 탄젠트
		const FVector EndLoc       = BlockOutLine->GetLocationAtSplinePoint(NextIdx, CoordSpace);
		const FVector EndTangent   = BlockOutLine->GetArriveTangentAtSplinePoint(NextIdx, CoordSpace); // 도착 탄젠트
		
		USplineMeshComponent* const NewSplineMesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), false, FTransform(), false));

		if (OB_IS_VALID(NewSplineMesh) == false)
			continue;

		NewSplineMesh->SetStaticMesh(OutLineMesh);
		NewSplineMesh->SetStartAndEnd(StartLoc, StartTangent, EndLoc, EndTangent);
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		NewSplineMesh->SetCollisionProfileName(EOmniCollisionProfile::SysDetectorOverlap);  // 버스 정류장 등 감지
		NewSplineMesh->SetCastShadow(false);
		NewSplineMesh->bCastDynamicShadow = false;
		NewSplineMesh->SetHiddenInGame(true);
		NewSplineMesh->SetGenerateOverlapEvents(true);

		if (IsValid(OutLineMat))
			NewSplineMesh->SetMaterial(0, OutLineMat);

		GeneratedMeshes.Push(NewSplineMesh);
	}

	SearchBusStop();
	SetTypeSignWidgetPosition();
}

void AOmniCityBlock::SearchBusStop()
{
	UClass* BusStopClass = AOmniStationBusStop::StaticClass();
	TArray<AActor*> AllOverlappingActors;

	for (TWeakObjectPtr<USplineMeshComponent>& GenMesh : GeneratedMeshes)
	{
		TArray<AActor*> PartOverLapping;
		FOmniStatics::GetOverlapActors(GenMesh.Get(), BusStopClass, PartOverLapping);

		AllOverlappingActors.Append(PartOverLapping);
	}

	// todo: cityBlock에서 지우고 탐색할 필요가? 버스 정류장에 일임하는 게 나을 수도 있음. 검토 필요
	// 범위기반 반복 중, OwnedBusStops이 수정될 수 있어서 임시 객체사용.
	TArray<TWeakObjectPtr<AOmniStationBusStop>> TempBusStops = OwnedBusStops;
	for (const TWeakObjectPtr<AOmniStationBusStop>& BusStopWeak : TempBusStops)
	{
		AOmniStationBusStop* BusStop = BusStopWeak.Get();
		if (BusStop)
			BusStop->UnlinkOwnerOmniCityBlock();
	}

	for (AActor* OverActor : AllOverlappingActors)
	{
		if (IsValid(OverActor) == false || OverActor->IsA(BusStopClass) == false)
			continue;

		AOmniStationBusStop* BusStop = Cast<AOmniStationBusStop>(OverActor);
		BusStop->UpdateOwnerOmniCityBlock(this); // 버스 정류장이 양방향 추가 로직 수행.
	}
	
}

FVector AOmniCityBlock::GetCentroidLocation() const
{
	FVector Centroid = FVector::Zero();

	const int32 SplinePointsSize = BlockOutLine->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < SplinePointsSize; ++PointIdx)
	{
		Centroid += BlockOutLine->GetLocationAtSplinePoint(PointIdx, ESplineCoordinateSpace::World);
	}

	Centroid /= static_cast<double>(SplinePointsSize);

	return Centroid;
}

void AOmniCityBlock::SetTypeSignWidgetPosition()
{
	// 외곽선 스플라인 포인트들의 산술평균으로 위치 지정.
	const FVector CentroidLocation = GetCentroidLocation();
	TypeSignWidgetComponent->SetWorldLocation(CentroidLocation);
}

void AOmniCityBlock::SetupBlockDescription(const ECityBlockType InCityBlock)
{
	CurrentType = InCityBlock;
	UOmniCityBlockWidget* TypeSignWidget = Cast<UOmniCityBlockWidget>(TypeSignWidgetComponent->GetUserWidgetObject());
	if (TypeSignWidget)
		TypeSignWidget->UpdateTypeState(CurrentType);
}

ECityBlockType AOmniCityBlock::GetCurrentBlockType() const
{
	return CurrentType;
}

int32 AOmniCityBlock::FindBusStopIdx(AOmniStationBusStop* InBusStop) const
{
	if(IsValid(InBusStop) == false)
		return INDEX_NONE;
	return OwnedBusStops.Find(InBusStop);
}

bool AOmniCityBlock::HasBusStop(AOmniStationBusStop* InBusStop) const
{
	return FindBusStopIdx(InBusStop) != INDEX_NONE;
}

void AOmniCityBlock::AddBusStop(AOmniStationBusStop* InBusStop)
{
	if (OB_IS_VALID(InBusStop))
	{
		if (HasBusStop(InBusStop) == false)
			OwnedBusStops.Emplace(InBusStop);
	}
}

void AOmniCityBlock::RemoveBusStop(AOmniStationBusStop* InBusStop)
{
	if (IsValid(InBusStop))
	{
		// 해당 버스 정류장 삭제할 때, 유효하지 않은 배열 요소도 같이 제거.
		OwnedBusStops.RemoveAll([&InBusStop](TWeakObjectPtr<AOmniStationBusStop>& Element)
		{
			const AOmniStationBusStop* Stop = Element.Get();
			return (Stop == InBusStop) || (Stop == nullptr);
		});
	}
}

void AOmniCityBlock::RemoveInvalidBusStop()
{
	// 유효하지 않거나, 연결되지 않은 버스 정류장 제거
	OwnedBusStops.RemoveAll([this](TWeakObjectPtr<AOmniStationBusStop>& Element)
	{
		const AOmniStationBusStop* Stop = Element.Get();
		if (Stop == nullptr)
			return true;
		if (Stop->GetOwnerOmniCityBlock() != this)
			return true;

		return false;
	});
}

TArray<TWeakObjectPtr<AOmniStationBusStop>> AOmniCityBlock::GetOwnedBusStops()
{
	return OwnedBusStops;
}

TArray<TWeakObjectPtr<AOmniStationBusStop>> AOmniCityBlock::FindNearbyBusStops()
{
	TArray<TWeakObjectPtr<AOmniStationBusStop>> NearbyBusStops;
	NearbyBusStops.Append(OwnedBusStops);

	for (TWeakObjectPtr<AOmniStationBusStop> StopWeak : OwnedBusStops)
	{
		// 정류장을 통해 인근 도로 획득
		const AOmniStationBusStop* OwnedStop = StopWeak.Get();
		if (OwnedStop == nullptr)
			continue;

		AOmniRoad* NearbyRoad                                      = OwnedStop->GetOwnerOmniRoad();
		TArray<TWeakObjectPtr<AOmniStationBusStop>> NearbyStopList = NearbyRoad->GetOwnedBusStops();

		// 인근 도로를 통해, 길 건너 버스 정류장 획득.
		for (TWeakObjectPtr<AOmniStationBusStop> NearbyStopWeak : NearbyStopList)
		{
			const AOmniStationBusStop* NearbyStop = NearbyStopWeak.Get();
			if (NearbyStop == nullptr)
				continue;

			if (NearbyStop->GetOwnerOmniCityBlock() != this)
				NearbyBusStops.Emplace(NearbyStopWeak);
		}
	}

	return NearbyBusStops;
}

FSubSector* AOmniCityBlock::GetSubSector(const int32 InSectorIdx)
{
	return SubSectorList.IsValidIndex(InSectorIdx) ? &SubSectorList[InSectorIdx] : nullptr;
}

bool AOmniCityBlock::IsNeighborBlock(AOmniCityBlock* InOther) const
{
	if (IsValid(InOther) == false)
		return false;

	for (TWeakObjectPtr<AOmniStationBusStop> BusStopWeak : OwnedBusStops)
	{
		const AOmniStationBusStop* BusStop = BusStopWeak.Get();
		if (BusStop == nullptr)
			continue;

		// 같은 도로를 공유하는 이웃 버스 정류장의 블록이 Other와 같은지 판단.
		TArray<TWeakObjectPtr<AOmniStationBusStop>> StopNeighborStopList = BusStop->GetNeighborStops();
		const TWeakObjectPtr<AOmniStationBusStop>* FindPtr = StopNeighborStopList.FindByPredicate([InOther](const TWeakObjectPtr<AOmniStationBusStop>& InNeighborStopWeak)
		{
			return InNeighborStopWeak->GetOwnerOmniCityBlock() == InOther;
		});
		if (FindPtr != nullptr)
			return true;
	}

	return false;
}

bool AOmniCityBlock::IsNeighborSector(const int32 MySectorIdx, AOmniCityBlock* InOtherBlock, const int32 InOtherSectorIdx) const
{
	if (SubSectorList.IsValidIndex(MySectorIdx) == false)
		return false;

	const FSubSector* OtherSubSector = InOtherBlock->GetSubSector(InOtherSectorIdx);
	if (OtherSubSector == nullptr)
		return false;

	const AOmniRoad* MySubSectorRoad    = SubSectorList[MySectorIdx].BusStop->GetOwnerOmniRoad();
	const AOmniRoad* OtherSubSectorRoad = OtherSubSector->BusStop->GetOwnerOmniRoad();

	return MySubSectorRoad == OtherSubSectorRoad;
}
