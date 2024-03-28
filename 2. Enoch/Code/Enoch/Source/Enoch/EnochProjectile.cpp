// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochProjectile.h"
#include "EnochActorFactory.h"
#include "Common/PJActs/ProjectileAct.h"
#include "EnochPaperFlipbookComponent.h"
#include "EnochField.h"
#include "EnochFieldCell.h"

AEnochProjectile::AEnochProjectile()
{
	;
}

void AEnochProjectile::BeginPlay()
{
	Super::BeginPlay();
}
void AEnochProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool AEnochProjectile::isValid()
{
	return GetData() != nullptr;
}

void AEnochProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto data = GetData();
	if (data == nullptr) return;
	auto loc = data->location;
	auto convertedLoc = AEnochField::GetLocation(loc.x, loc.y);
	SetActorLocation(convertedLoc);
}

void AEnochProjectile::Bomb()
{
	if (Flipbook == nullptr) return;
	Flipbook->Bomb();
	Flipbook->OnFinishedPlaying.Clear();
	Flipbook->OnFinishedPlaying.AddDynamic(this, &AEnochProjectile::BombEnd);
}

void AEnochProjectile::BombEnd()
{
	AEnochActorFactory::Release(this);
}

void AEnochProjectile::InitPJ()
{
	auto data = GetData();
	
}

EnochProjectileData* AEnochProjectile::GetData()
{
	return AEnochActorFactory::GetProjectileData(SerialNumber);
}