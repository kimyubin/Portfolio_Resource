// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelRampart.h"

#include "CitadelForever.h"
#include "CitadelStatics.h"
#include "CitadelTower.h"

ACitadelRampart::ACitadelRampart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(ECitadelTag::CitadelRampart);

	TowerInstallPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TowerInstallPoint"));
	TowerInstallPointComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	TowerInstallPointComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

}

void ACitadelRampart::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACitadelRampart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ACitadelRampart::TrackCursorForPreview(const FVector& InLocation, AActor* InHitActor)
{
	bool Installable = false;
	bOnRampart = false;
	
	//성벽 찍으면 그 자리에서 위치한 다음 설치 불가 판정.
	if (InHitActor->ActorHasTag(ECitadelTag::CitadelRampart))
	{
		MyBuildingInfo.Location = InHitActor->GetActorLocation();
		Installable = false;
	}
	else if (InHitActor->ActorHasTag(ECitadelTag::CitadelTower))
	{
		Installable = false;
		
		//타워 찍으면 그자리에, 성벽 위에 있는 타워면 아무것도 안함.
		if(InHitActor->IsA(ACitadelTower::StaticClass()))
		{			
			if (!IsValid(Cast<ACitadelTower>(InHitActor)->GetOwnerRampart()))
			{			
				MyBuildingInfo.Location = InHitActor->GetActorLocation();
			}
		}		
	}
	else
	{
		MyBuildingInfo.Location = FVector(UCitadelStatics::RoundUpForUnit(InLocation.X), UCitadelStatics::RoundUpForUnit(InLocation.Y), InLocation.Z);
		Installable = true;
	}	

	if (!bOverlapOtherBuilding && Installable)
	{
		ChangeInstallableState(true);
	}
	else
	{
		ChangeInstallableState(false);
	}	
	
	SetActorLocation(MyBuildingInfo.Location);
}

void ACitadelRampart::InstallTowerOnRampart(ACitadelTower* InInstallTargetTower)
{
	MyInstalledTower = InInstallTargetTower;
	MyInstalledTower->SetOwnerRampart(this);	
}

FVector ACitadelRampart::GetInstallLocation() const
{
	return TowerInstallPointComponent->GetComponentTransform().GetLocation();
}

bool ACitadelRampart::HasTower() const
{
	if(MyInstalledTower.IsValid())
		return true;
	return false;
}
