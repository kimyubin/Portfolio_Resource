// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelTower.h"

#include "CitadelRampart.h"
#include "CitadelStatics.h"

ACitadelTower::ACitadelTower()
{ 	
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(ECitadelTag::CitadelTower);
}

void ACitadelTower::BeginPlay()
{
	Super::BeginPlay();	
}

void ACitadelTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACitadelTower::TrackCursorForPreview(const FVector& InLocation, AActor* InHitActor)
{
	if (InHitActor->ActorHasTag(ECitadelTag::CitadelRampart))
	{
		//성벽 위
		MyBuildingInfo.Location = Cast<ACitadelRampart>(InHitActor)->GetInstallLocation();

		if (Cast<ACitadelRampart>(InHitActor)->HasTower())
		{
			//성벽에 타워 있음
			bOnRampart = false;
			ChangeInstallableState(false);
		}
		else
		{
			//성벽에 타워 없음
			bOnRampart = true;
			MyOwnerCitadelRampart = Cast<ACitadelRampart>(InHitActor);
			ChangeInstallableState(true);
		}
	}
	else
	{
		bOnRampart = false;
		MyOwnerCitadelRampart.Reset();
		
		//타워를 찍으면 그 자리에서 위치한 다음 설치 불가 판정.
		if (InHitActor->ActorHasTag(ECitadelTag::CitadelTower))
		{
			MyBuildingInfo.Location = InHitActor->GetActorLocation();			
		}
		else
		{
			MyBuildingInfo.Location = FVector(UCitadelStatics::RoundUpForUnit(InLocation.X), UCitadelStatics::RoundUpForUnit(InLocation.Y), InLocation.Z);
		}

		if (bOverlapOtherBuilding)
		{
			ChangeInstallableState(false);
		}
		else
		{
			ChangeInstallableState(true);
		}
	}

	SetActorLocation(MyBuildingInfo.Location);
}


void ACitadelTower::SearchAndAttackEnemy()
{
	
}

ACitadelRampart* ACitadelTower::GetOwnerRampart()
{
	if(MyOwnerCitadelRampart.IsValid())
		return MyOwnerCitadelRampart.Get();
	return nullptr;
}

void ACitadelTower::SetOwnerRampart(ACitadelRampart* InRampart)
{
	MyOwnerCitadelRampart = InRampart;
}
