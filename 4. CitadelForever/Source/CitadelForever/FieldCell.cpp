// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FieldCell.h"

// Sets default values
AFieldCell::AFieldCell()
{
	PrimaryActorTick.bCanEverTick = true;

	CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = CellMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CELL(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));	
	if (CELL.Succeeded())
	{
		CellMesh->SetStaticMesh(CELL.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> DEFAULT_MAT(TEXT("/Game/StarterContent/Materials/M_Brick_Clay_Old.M_Brick_Clay_Old"));
	if (DEFAULT_MAT.Succeeded())
	{
		CellMesh->SetMaterial(0, DEFAULT_MAT.Object);
	}	

}

// Called when the game starts or when spawned
void AFieldCell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFieldCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

