// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniStationBusStop.h"

#include "OmniAsync.h"
#include "OmnibusTypes.h"
#include "OmniCityBlock.h"
#include "OmniLineBusRoute.h"
#include "OmniPassenger.h"
#include "OmniRoad.h"
#include "OmniRoadTwoLane.h"
#include "OmniVehicleBus.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"

#include "UtlLog.h"
#include "UTLStatics.h"

AOmniStationBusStop::AOmniStationBusStop()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static);

	BusStopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BusStopMesh"));
	BusStopMesh->SetupAttachment(RootComponent);
	BusStopMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BusStopMesh->SetMobility(EComponentMobility::Static);
	BusStopMesh->SetCastShadow(false);

	InputMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputMesh"));
	InputMeshComponent->SetupAttachment(RootComponent);
	InputMeshComponent->SetCollisionProfileName(EOmniCollisionProfile::LineTraceOnly);
	InputMeshComponent->SetVisibility(false, false);
	InputMeshComponent->SetHiddenInGame(true);

	StationDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("StationDetector"));
	StationDetector->SetupAttachment(RootComponent);
	StationDetector->SetCollisionProfileName(EOmniCollisionProfile::SysDetectorOverlap);
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
	
	FOmniAsync::UpdateStopProxyAsync(this);
}

void AOmniStationBusStop::Destroyed()
{
	//레벨 전환시에는 상위 도로도 삭제되니, 명시적으로 제거될 때만 호출
	UnlinkOwnerOmniRoad();

	Super::Destroyed();
}

void AOmniStationBusStop::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FOmniAsync::DeleteStopProxyAsync(this);

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

	// 게임 시작 후, 도로에 붙어있는 지 확인.
	if (GetWorld() != nullptr && GetWorld()->GetBegunPlay() == true)
	{
		if (UT_IS_WEAK_PTR_VALID(OwnerOmniRoad) == false)// || UT_IS_WEAK_PTR_VALID(OwnerOmniCityBlock) == false)
		{
			UT_LOG("%s No Road or Block Detected", *GetActorNameOrLabel())
		}
	}
}

void AOmniStationBusStop::SearchRoad()
{
	FOmniAsync::UpdateStopProxyAsync(this);

	AOmniRoad* NearRoad        = nullptr;
	OwnerLaneIndex             = 0;
	const bool bIsDetectedRoad = DetectRoadAndLane(NearRoad, OwnerLaneIndex);

	// 감지 실패 시 연결 해제.
	if (bIsDetectedRoad == false || UT_IS_VALID(NearRoad) == false)
	{
		UnlinkOwnerOmniRoad();
		return;
	}

	// 도로 연결 후 Transform 조정
	UpdateOwnerOmniRoad(NearRoad);

	// 정류장은 레벨에 스폰된 상태로 시작합니다.
	// 위치와 회전은 에디팅 단계에서만 작동하고, 게임 플레이에서는 작동하지 않습니다.
	if (GetWorld()->IsGameWorld() == false)
	{
		const FVector MyLocation               = GetActorLocation();
		const USplineComponent* Lane           = NearRoad->GetLaneSpline(OwnerLaneIndex);
		ClosestLaneInputKey                    = Lane->FindInputKeyClosestToWorldLocation(MyLocation);
		ClosestLanePointLocation               = Lane->GetLocationAtSplineInputKey(ClosestLaneInputKey, ESplineCoordinateSpace::World);
		const FVector ClosestSplineRightVector = Lane->GetRightVectorAtSplineInputKey(ClosestLaneInputKey, ESplineCoordinateSpace::World);
		const FVector ClosestSplineDirection   = Lane->GetDirectionAtSplineInputKey(ClosestLaneInputKey, ESplineCoordinateSpace::World);

		const double Offset       = NearRoad->GetRoadWidth() / 4.0; // 차선폭의 절반(도로의 1/4)
		const FVector NewLocation = ClosestLanePointLocation + (ClosestSplineRightVector * Offset);
		const FRotator NewRotator = FVector(-ClosestSplineDirection.X, -ClosestSplineDirection.Y, 0.0).Rotation();

		SetActorLocation(FVector(NewLocation.X, NewLocation.Y, 0.0));
		SetActorRotation(NewRotator);
	}
}

void AOmniStationBusStop::SearchCityBlock()
{
	FOmniAsync::UpdateStopProxyAsync(this);
	
	UClass* CityBlockClass        = AOmniCityBlock::StaticClass();
	AOmniCityBlock* NearCityBlock = nullptr;
	const bool IsDetectCityBlock  = DetectCityBlock(CityBlockClass, NearCityBlock);

	// 감지 실패 시 연결 해제.
	if (IsDetectCityBlock == false || UT_IS_VALID(NearCityBlock) == false || NearCityBlock->IsA(CityBlockClass) == false)
	{
		UnlinkOwnerOmniCityBlock();
		return;
	}

	UpdateOwnerOmniCityBlock(NearCityBlock);
}

bool AOmniStationBusStop::DetectRoadAndLane(AOmniRoad*& OutNearRoad, int32& OutNearLaneIdx) const
{
	TArray<AActor*> OverlappingActors;
	const bool bIsOverlap = FUtlStatics::GetOverlapActors(StationDetector, AOmniRoad::StaticClass(), OverlappingActors);
	if (bIsOverlap == false)
		return false;

	// OmniRoad 아니면 제거
	// 정류장 설치 불가면 제거
	OverlappingActors.RemoveAllSwap([](const AActor* InActor)
	{
		const AOmniRoad* InRoad = Cast<AOmniRoad>(InActor);
		return InRoad ? (InRoad->CanInstallBusStop() == false) : true;
	});

	return AOmniRoad::FindNearestRoadAndLane(OverlappingActors
	                                       , GetActorLocation()
	                                       , OutNearRoad
	                                       , OutNearLaneIdx);
}

bool AOmniStationBusStop::DetectCityBlock(UClass* InClassFilter, AOmniCityBlock*& OutCityBlock) const
{
	OutCityBlock = nullptr;
	TArray<AActor*> OverlappingActors;
	const bool IsOverlap = FUtlStatics::GetOverlapActors(StationDetector, InClassFilter, OverlappingActors);

	if (IsOverlap == false)
		return false;

	if (OverlappingActors.Num() >= 2)
		UT_LOG("A BusStop has detected multiple CityBlocks.")

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
	if (UT_IS_VALID(InOwnerOmniRoad) == false)
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
	if (UT_IS_VALID(InOwnerOmniCityBlock) == false)
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

void AOmniStationBusStop::AddBusRoute(AOmniLineBusRoute* InRoute)
{
	FOmniAsync::UpdateStopProxyAsync(this);

	BusRouteList.Emplace(InRoute);
}

void AOmniStationBusStop::RemoveBusRoute(AOmniLineBusRoute* InRoute)
{
	FOmniAsync::UpdateStopProxyAsync(this);

	// 버스노선 제거하는 김에 유효하지 않은 버스 노선 제거.
	if (IsValid(InRoute))
	{
		BusRouteList.RemoveAll([&InRoute](TWeakObjectPtr<AOmniLineBusRoute>& Element)
		{
			return (Element == InRoute) || (Element.IsValid() == false);
		});
	}
}

TArray<TWeakObjectPtr<AOmniStationBusStop>> AOmniStationBusStop::GetNeighborStops() const
{
	TArray<TWeakObjectPtr<AOmniStationBusStop>> Results;

	AOmniRoad* OwnerRoad = OwnerOmniRoad.Get();
	if (OwnerRoad == nullptr)
		return Results;

	return OwnerRoad->GetOwnedBusStops();
}

void AOmniStationBusStop::StartEntryToBus(AOmniVehicleBus* InBus)
{
	TArray<AOmniPassenger*> EntryPassengerList;
	WaitPassengerList.RemoveAll([&EntryPassengerList, InBus](const TWeakObjectPtr<AOmniPassenger>& PassengerWeak)
	{
		// Invalid 제거.
		AOmniPassenger* Passenger = PassengerWeak.Get();
		if (Passenger == nullptr)
			return true;

		// 이번에 탑승할 사람 탑승자 목록에 넣고, 대기 목록에서 삭제. 
		if (Passenger->IsEntryToThisBus(InBus))
		{
			EntryPassengerList.Emplace(Passenger);
			return true;
		}
		return false;
	});

	// 단위 승차 시간(하차 속도의 역수)
	const float UnitEntryDelay = 1.0f / InBus->GetExitSpeed();
	for (int idx = 0; idx < EntryPassengerList.Num(); ++idx)
	{
		// 각각 일정 시간 간격으로 승차 절차 밟음.
		const float EntryDelay = static_cast<float>(idx + 1) * UnitEntryDelay;

		AOmniPassenger* ExitPassenger = EntryPassengerList[idx];
		ExitPassenger->DoEntryToBus(InBus, EntryDelay);
	}

	// 승차 완료 예정 시간 이후 발차. 승차자가 없다면 즉시 시전.
	if (EntryPassengerList.IsEmpty())
	{
		InBus->EndBoarding();
	}
	else
	{
		const float EntryTime = static_cast<float>(EntryPassengerList.Num() + 1) * UnitEntryDelay;
		FTimerHandle TempTimer;
		GetWorldTimerManager().SetTimer(TempTimer, [InBus]()
		{
			InBus->EndBoarding();
		}, EntryTime, false);
	}
}

FTransform AOmniStationBusStop::WaitAtStopLine(AOmniPassenger* InPassenger)
{
	WaitPassengerList.Emplace(InPassenger);
	
	// todo: 대기줄 자리 대신 일단 버스 정류장 위치
	return GetActorTransform();
}
