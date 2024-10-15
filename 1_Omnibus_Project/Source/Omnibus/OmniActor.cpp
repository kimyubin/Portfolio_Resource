// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniActor.h"

#include "OmnibusGameInstance.h"
#include "OmnibusUtilities.h"

AOmniActor::AOmniActor()
{
	PrimaryActorTick.bCanEverTick = true;
	MyOmniID = 0;
}

void AOmniActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetOmniID();
}

void AOmniActor::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniActor::PostBeginPlay()
{
}

void AOmniActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniActor::SetOmniID()
{
	MyOmniID = UOmniID::GenerateID_Number(this);
}

uint64 AOmniActor::GetOmniID()
{
	if (MyOmniID == 0)
		SetOmniID();

	return MyOmniID;
}

void AOmniActor::SetZ_Axis()
{
	FVector TempActorLocation = GetActorLocation();
	if (TempActorLocation.Z != 0.0)
	{
		TempActorLocation.Z = 0.0;
		SetActorLocation(TempActorLocation);
	}
}

UOmnibusGameInstance* AOmniActor::GetOmniGameInstance() const
{
	return GetGameInstance<UOmnibusGameInstance>();
}

UOmnibusPlayData* AOmniActor::GetOmnibusPlayData() const
{
	return GetOmniGameInstance()->GetOmnibusPlayData();
}

UOmnibusUIsHandler* AOmniActor::GetOmnibusUIsHandler() const
{
	return GetOmniGameInstance()->GetOmnibusUIsHandler();
}

AOmnibusRoadManager* AOmniActor::GetOmnibusRoadManager() const
{
	return GetOmniGameInstance()->GetOmnibusRoadManager();
}

UOmniTimeManager* AOmniActor::GetOmniTimeManager() const
{
	return GetOmniGameInstance()->GetOmniTimeManager();
}
