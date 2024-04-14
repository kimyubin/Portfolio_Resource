// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniStationBusStop.h"

#include "OmniRoad.h"
#include "Components/BoxComponent.h"

AOmniStationBusStop::AOmniStationBusStop()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BusStopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BusStopMesh"));
	BusStopMesh->SetupAttachment(RootComponent);

	DecelerationArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DecelerationArea"));
	DecelerationArea->SetupAttachment(RootComponent);

	BusStopArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BusStopArea"));
	BusStopArea->SetupAttachment(RootComponent);
}

void AOmniStationBusStop::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniStationBusStop::Destroyed()
{
	//레벨 전환시에는 상위 도로도 삭제되니, 명시적으로 제거될 때만 호출
	if (OwnerOmniRoad.IsValid())
		OwnerOmniRoad->RemoveOwnedBusStop(GetOmniID());

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

void AOmniStationBusStop::SetOwnerOmniRoad(AOmniRoad* InOwnerOmniRoad)
{
	OwnerOmniRoad = InOwnerOmniRoad;
}

AOmniRoad* AOmniStationBusStop::GetOwnerOmniRoad() const
{
	if (OwnerOmniRoad.IsValid())
		return OwnerOmniRoad.Get();
	
	return nullptr;
}
