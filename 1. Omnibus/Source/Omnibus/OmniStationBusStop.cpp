// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniStationBusStop.h"

#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniCityBlock.h"
#include "OmniRoad.h"
#include "OmniRoadDefaultTwoLane.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AOmniStationBusStop::AOmniStationBusStop()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BusStopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BusStopMesh"));
	BusStopMesh->SetupAttachment(RootComponent);

	StationDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("StationDetector"));
	StationDetector->SetupAttachment(RootComponent);
	StationDetector->SetCollisionProfileName(EOmniCollisionProfile::SysDetector);
	
	DecelerationArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DecelerationArea"));
	DecelerationArea->SetupAttachment(RootComponent);

	BusStopArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BusStopArea"));
	BusStopArea->SetupAttachment(RootComponent);
}

void AOmniStationBusStop::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SearchRoadAndBlock();
}

#if WITH_EDITOR
void AOmniStationBusStop::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	SetZ_Axis();
	SearchRoadAndBlock();
}
#endif // WITH_EDITOR

void AOmniStationBusStop::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniStationBusStop::Destroyed()
{
	//레벨 전환시에는 상위 도로도 삭제되니, 명시적으로 제거될 때만 호출
	UnlinkOwnerOmniRoad();

	Super::Destroyed();
}

void AOmniStationBusStop::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOmniStationBusStop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniStationBusStop::SearchRoadAndBlock()
{
	SearchRoad();
	SearchCityBlock();
}

void AOmniStationBusStop::SearchRoad()
{
	UClass* TargetClass       = AOmniRoadDefaultTwoLane::StaticClass();
	AOmniRoad* NearRoad       = nullptr;
	OwnerLaneIndex            = 0;
	const bool IsDetectedRoad = DetectRoadAndLane(TargetClass, NearRoad, OwnerLaneIndex);

	// 감지 실패 시 연결 해제.
	if (IsDetectedRoad == false || OB_IS_VALID(NearRoad) == false || NearRoad->IsA(TargetClass) == false)
	{
		UnlinkOwnerOmniRoad();
		return;
	}

	// 도로 연결 후 Transform 조정
	UpdateOwnerOmniRoad(NearRoad);
	const FVector MyLocation               = GetActorLocation();
	const USplineComponent* Lane           = NearRoad->GetLaneSpline(OwnerLaneIndex);
	ClosestLaneInputKey                    = Lane->FindInputKeyClosestToWorldLocation(MyLocation);
	ClosestLanePointLocation               = Lane->GetLocationAtSplineInputKey(ClosestLaneInputKey, ESplineCoordinateSpace::World);
	const FVector ClosestSplineRightVector = Lane->GetRightVectorAtSplineInputKey(ClosestLaneInputKey, ESplineCoordinateSpace::World);
	const FVector ClosestSplineDirection   = Lane->GetDirectionAtSplineInputKey(ClosestLaneInputKey, ESplineCoordinateSpace::World);

	const double Offset       = NearRoad->GetRoadWidth() / 4.0; // 차선폭의 절반
	const FVector NewLocation = ClosestLanePointLocation + (ClosestSplineRightVector * Offset);
	const FRotator NewRotator = FVector(-ClosestSplineDirection.X, -ClosestSplineDirection.Y, 0.0).Rotation();

	SetActorLocation(FVector(NewLocation.X, NewLocation.Y, 0.0));
	SetActorRotation(NewRotator);
}

void AOmniStationBusStop::SearchCityBlock()
{
	UClass* CityBlockClass        = AOmniCityBlock::StaticClass();
	AOmniCityBlock* NearCityBlock = nullptr;
	const bool IsDetectCityBlock  = DetectCityBlock(CityBlockClass, NearCityBlock);

	// 감지 실패 시 연결 해제.
	if (IsDetectCityBlock == false || OB_IS_VALID(NearCityBlock) == false || NearCityBlock->IsA(CityBlockClass) == false)
	{
		UnlinkOwnerOmniCityBlock();
		return;
	}

	UpdateOwnerOmniCityBlock(NearCityBlock);
}

bool AOmniStationBusStop::DetectRoadAndLane(UClass* InClassFilter, AOmniRoad*& OutNearRoad, uint32& OutNearLaneIdx) const
{
	TArray<AActor*> OverlappingActors;
	const bool IsOverlap = FOmniStatics::GetOverlapActors(StationDetector, InClassFilter, OverlappingActors);

	if (IsOverlap == false)
		return false;

	return AOmniRoad::FindNearestRoadAndLane(OverlappingActors
	                                       , InClassFilter
	                                       , GetActorLocation()
	                                       , OutNearRoad
	                                       , OutNearLaneIdx);
}

bool AOmniStationBusStop::DetectCityBlock(UClass* InClassFilter, AOmniCityBlock*& OutCityBlock) const
{
	OutCityBlock = nullptr;
	TArray<AActor*> OverlappingActors;
	const bool IsOverlap = FOmniStatics::GetOverlapActors(StationDetector, InClassFilter, OverlappingActors);

	if (IsOverlap == false)
		return false;

	AActor** FindPtr = OverlappingActors.FindByPredicate([InClassFilter](const AActor* InOverlap) -> bool
	{
		return InOverlap->IsA(InClassFilter);
	});
	
	if (FindPtr == nullptr)
		return false;

	OutCityBlock = Cast<AOmniCityBlock>(*FindPtr);
	return true;
}

AOmniRoad* AOmniStationBusStop::GetOwnerOmniRoad() const
{
	return OwnerOmniRoad.Get();
}

void AOmniStationBusStop::UpdateOwnerOmniRoad(AOmniRoad* InOwnerOmniRoad)
{
	if (OB_IS_VALID(InOwnerOmniRoad) == false)
		return;
		
	// 옮기면 이전 도로 연결 해제
	if (OwnerOmniRoad.IsValid() && OwnerOmniRoad != InOwnerOmniRoad)
		OwnerOmniRoad->RemoveBusStop(this);

	OwnerOmniRoad = InOwnerOmniRoad;
	OwnerOmniRoad->AddBusStop(this);
}

void AOmniStationBusStop::UnlinkOwnerOmniRoad()
{
	if (OwnerOmniRoad.IsValid())
		OwnerOmniRoad->RemoveBusStop(this);

	OwnerOmniRoad = nullptr;
}

AOmniCityBlock* AOmniStationBusStop::GetOwnerOmniCityBlock() const
{
	return OwnerOmniCityBlock.Get();
}

void AOmniStationBusStop::UpdateOwnerOmniCityBlock(AOmniCityBlock* InOwnerOmniCityBlock)
{
	if (OB_IS_VALID(InOwnerOmniCityBlock) == false)
		return;

	// 옮기면 이전 도로 연결 해제
	if (OwnerOmniCityBlock.IsValid() && OwnerOmniCityBlock != InOwnerOmniCityBlock)
		OwnerOmniCityBlock->RemoveBusStop(this);

	OwnerOmniCityBlock = InOwnerOmniCityBlock;
	OwnerOmniCityBlock->AddBusStop(this);
}

void AOmniStationBusStop::UnlinkOwnerOmniCityBlock()
{
	if (OwnerOmniCityBlock.IsValid())
		OwnerOmniCityBlock->RemoveBusStop(this);

	OwnerOmniCityBlock = nullptr;
}
