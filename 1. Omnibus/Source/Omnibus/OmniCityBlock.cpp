// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniCityBlock.h"

#include "Omnibus.h"
#include "OmnibusRoadManager.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniCityBlockWidget.h"
#include "OmniStationBusStop.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TextRenderComponent.h"
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

void AOmniCityBlock::MakeCollisionAlongSpline()
{
	if (OB_IS_VALID(PlacedMesh) == false)
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

		NewSplineMesh->SetStaticMesh(PlacedMesh);
		NewSplineMesh->SetStartAndEnd(StartLoc, StartTangent, EndLoc, EndTangent);
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
		NewSplineMesh->SetCollisionProfileName(EOmniCollisionProfile::SysDetector);  // 버스 정류장 등 감지
		NewSplineMesh->SetHiddenInGame(true);
		NewSplineMesh->SetGenerateOverlapEvents(true);
		if (IsValid(PlacedMat))
			NewSplineMesh->SetMaterial(0, PlacedMat);

		GeneratedMeshes.Push(NewSplineMesh);
	}

	SearchBusStop();
	CalculateTypeSignPosition();
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
	for(const auto& BusStop : TempBusStops)
	{
		BusStop->UnlinkOwnerOmniCityBlock();
	}
	
	for (const auto& OverActor : AllOverlappingActors)
	{
		if (IsValid(OverActor) == false || OverActor->IsA(BusStopClass) == false)
			continue;

		AOmniStationBusStop* BusStop = Cast<AOmniStationBusStop>(OverActor);
		BusStop->UpdateOwnerOmniCityBlock(this); // 버스 정류장이 양방향 추가 로직 수행.
	}
	
}

void AOmniCityBlock::CalculateTypeSignPosition()
{
	// 외곽선 스플라인 포인트들의 산술평균으로 위치 지정.
	FVector CenterLocation = FVector::Zero();

	const int32 SplinePointsSize = BlockOutLine->GetNumberOfSplinePoints();
	for (int PointIdx = 0; PointIdx < SplinePointsSize; ++PointIdx)
	{
		CenterLocation += BlockOutLine->GetLocationAtSplinePoint(PointIdx, ESplineCoordinateSpace::World);
	}

	CenterLocation /= static_cast<double>(SplinePointsSize);
	TypeSignWidgetComponent->SetWorldLocation(CenterLocation);
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
			return (Element == InBusStop) || (Element.IsValid() == false);
		});
	}
}

void AOmniCityBlock::RemoveInvalidBusStop()
{
	// 유효하지 않거나, 연결되지 않은 버스 정류장 제거
	OwnedBusStops.RemoveAll([this](TWeakObjectPtr<AOmniStationBusStop>& Element)
	{
		if (Element.IsValid() == false)
			return true;
		if (Element.Get()->GetOwnerOmniCityBlock() != this)
			return true;
		return false;
	});
}
