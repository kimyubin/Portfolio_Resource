// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniPawn.h"

#include "OmnibusGameInstance.h"
#include "OmnibusUtilities.h"

AOmniPawn::AOmniPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	MyOmniID = 0;
}

void AOmniPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetOmniID();
}

void AOmniPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniPawn::PostBeginPlay()
{
}

void AOmniPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOmniPawn::SetOmniID()
{
	MyOmniID = UOmniID::GenerateID_Number(this);
}

uint64 AOmniPawn::GetOmniID()
{
	if (MyOmniID == 0)
		SetOmniID();

	return MyOmniID;
}

UOmnibusGameInstance* AOmniPawn::GetOmniGameInstance() const
{
	return GetGameInstance<UOmnibusGameInstance>();
}

UOmnibusPlayData* AOmniPawn::GetOmnibusPlayData() const
{
	return GetOmniGameInstance()->GetOmnibusPlayData();
}

UOmnibusUIsHandler* AOmniPawn::GetOmnibusUIsHandler() const
{
	return GetOmniGameInstance()->GetOmnibusUIsHandler();
}

AOmnibusRoadManager* AOmniPawn::GetOmnibusRoadManager() const
{
	return GetOmniGameInstance()->GetOmnibusRoadManager();
}

UOmniTimeManager* AOmniPawn::GetOmniTimeManager() const
{
	return GetOmniGameInstance()->GetOmniTimeManager();
}
