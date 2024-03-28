// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniActor.h"
#include "OmnibusUtilities.h"

AOmniActor::AOmniActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOmniActor::BeginPlay()
{
	Super::BeginPlay();
}

void AOmniActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetOmniID();
}

void AOmniActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniActor::SetOmniID()
{
	if (MyOmniID == 0)
		MyOmniID = UOmniID::GenerateID_Number();
}

uint64 AOmniActor::GetOmniID()
{
	if (MyOmniID == 0)
		SetOmniID();
	return MyOmniID;
}
