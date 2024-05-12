// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniPawn.h"
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
