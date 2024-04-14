// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniLaneApproachCollision.h"

#include "OmnibusUtilities.h"
#include "OmniRoad.h"
#include "OmniVehicleBus.h"
#include "Components/SplineComponent.h"

UOmniLaneApproachCollision::UOmniLaneApproachCollision()
{
	LaneApproachIdx = -1;

}

void UOmniLaneApproachCollision::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerOmniRoad.IsValid() == false)
		OB_LOG("OwnerOmniRoad is not valid.")
	
	if (LaneApproachIdx < 0)
		OB_LOG("LaneApproachIdx is invalid value. less than 0.")

	// OnComponentBeginOverlap.AddDynamic(this, &UOmniLaneApproachCollision::ApproachBeginOverlap);
}

void UOmniLaneApproachCollision::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UOmniLaneApproachCollision::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOmniLaneApproachCollision::ApproachBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AOmniVehicleBus::StaticClass()))
	{
		if (OwnerOmniRoad.IsValid() == true)
		{
		}
	}
}

void UOmniLaneApproachCollision::InitLaneApproach(AOmniRoad* InOwnerOmniRoad, const int32 InApproachIdx)
{
	OwnerOmniRoad   = InOwnerOmniRoad;
	LaneApproachIdx = InApproachIdx;
}

USplineComponent* UOmniLaneApproachCollision::GetNextSplineByRoad(AOmniRoad* InNextTargetRoad) const
{
	// if (OwnerOmniRoad.IsValid() == true)
	// 	return OwnerOmniRoad->GetSplineToNextRoad(LaneApproachIdx, InNextTargetRoad);

	return nullptr;
}
