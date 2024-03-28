// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelField.h"

// Sets default values
ACitadelField::ACitadelField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACitadelField::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACitadelField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

