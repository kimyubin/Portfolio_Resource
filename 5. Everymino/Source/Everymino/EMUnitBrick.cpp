// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMUnitBrick.h"

#include "EMGameInstance.h"
#include "EMPlayData.h"

AEMUnitBrick::AEMUnitBrick()
{
	PrimaryActorTick.bCanEverTick = true;
	MySkinNum = UnitSkin::Default;
	CubeStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeStaticMesh"));
	RootComponent = CubeStaticMeshComponent;
}

void AEMUnitBrick::BeginPlay()
{
	Super::BeginPlay();
}

void AEMUnitBrick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEMUnitBrick::SetSkinMesh(UnitSkin SkinEnum)
{
	UStaticMesh* Mesh = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetUnitSkinMesh(SkinEnum);
	CubeStaticMeshComponent->SetStaticMesh(Mesh);
}
void AEMUnitBrick::SetSkinMesh(uint8 SkinNum)
{		
	SetSkinMesh(static_cast<UnitSkin>(EnumToInt(SkinNum)));
}

void AEMUnitBrick::SetPositionAndSkin(int InX, int InY, UnitSkin SkinEnum)
{	
	SetPositionAndSkin(FVector2I(InX,InY), SkinEnum);
}
void AEMUnitBrick::SetPositionAndSkin(FVector2I InPosition, UnitSkin SkinEnum)
{
	SetSkinMesh(SkinEnum);
	SetPosition(InPosition);
}

void AEMUnitBrick::SetPosition(int InX, int InY)
{
	SetPosition(FVector2I(InX, InY));
}
void AEMUnitBrick::SetPosition(const FVector2I& InPosition)
{

	//유닛 사이즈 간격으로 벌림.
	// 전부 부모 액터 = BrickBoardManager 기준 상대 위치로 변경
	// 맨 위에서부터 읽는데, 숫자가 커질수록 위로 올라감. = 위로 올가가면서 그림 = 거꾸로 스폰.
	// 역수를 곱해줘서 위아래 뒤집음.

	MyPosition = InPosition;

	const int CalX = (MyPosition.X * BrickTemplate::UNIT_SIZE);	
	const int CalY = (MyPosition.Y * - BrickTemplate::UNIT_SIZE);

	SetActorRelativeLocation(FVector(CalX, 0, CalY));
	SetActorRelativeRotation(FRotator::ZeroRotator);
}
