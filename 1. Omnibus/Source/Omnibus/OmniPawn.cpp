// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniPawn.h"

#include "OmnibusUtilities.h"

AOmniPawn::AOmniPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AOmniPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetOmniID();
}

void AOmniPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOmniPawn::SetOmniID()
{
	if (MyOmniID == 0)
		MyOmniID = UOmniID::GenerateID_Number();
}

uint64 AOmniPawn::GetOmniID()
{
	if (MyOmniID == 0)
		SetOmniID();
	
	return MyOmniID;
}
