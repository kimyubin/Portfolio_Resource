// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelBuildingFactory.h"
#include "CitadelRampart.h"
#include "CitadelTower.h"
#include "CitadelBuilding.h"
#include "CitadelForeverGameInstance.h"
#include "CitadelPlayData.h"
#include "Kismet/GameplayStatics.h"

ACitadelBuildingFactory::ACitadelBuildingFactory()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACitadelBuildingFactory::BeginPlay()
{
	Super::BeginPlay();

	// 레벨이 시작할 때, 이미 깔려있는 빌딩 리스트 수집.
	TArray<AActor*> BuildingList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACitadelBuilding::StaticClass(),BuildingList);
	for (const auto& SpawnedBuilding : BuildingList)
	{
		if(SpawnedBuilding->IsA(ACitadelBuilding::StaticClass()))
			InstalledBuildingList.Add(Cast<ACitadelBuilding>(SpawnedBuilding));	
	}
}

void ACitadelBuildingFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ACitadelBuilding* ACitadelBuildingFactory::SpawnPreviewBuilding(FBuildingInfo InBuildingInfo)
{
	const auto BuildingClass =
	GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData()->GetBuildingClass(InBuildingInfo.BuildingType);
	
	BF_Weak_PreviewBuilding = GetWorld()->SpawnActor<ACitadelBuilding>(BuildingClass, InBuildingInfo.Location,InBuildingInfo.Rotator);

	BF_Weak_PreviewBuilding->MyBuildingInfo = InBuildingInfo;

	BF_Weak_PreviewBuilding.Get()->RunPreviewMode();
	
	return BF_Weak_PreviewBuilding.Get();
}

bool ACitadelBuildingFactory::BuildPreviewToReal()
{	
	if(!BF_Weak_PreviewBuilding.IsValid())
		return false;
	if(!BF_Weak_PreviewBuilding->CanInstall())
		return false;
	
	FBuildingInfo NewBuildingInfo = BF_Weak_PreviewBuilding->MyBuildingInfo;

	const auto BuildingClass =
		GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData()->GetBuildingClass(NewBuildingInfo.BuildingType);
	const auto InstalledBuilding =
		GetWorld()->SpawnActor<ACitadelBuilding>(BuildingClass, NewBuildingInfo.Location, NewBuildingInfo.Rotator);

	//성벽 위에 설치 체크
	if(BF_Weak_PreviewBuilding->IsA(ACitadelTower::StaticClass()))
	{
		if(BF_Weak_PreviewBuilding->IsOnRampart())
		{
			ACitadelRampart* UnderRampart = Cast<ACitadelTower>(BF_Weak_PreviewBuilding)->GetOwnerRampart();
			if(IsValid(UnderRampart))
			{
				auto CastTower = Cast<ACitadelTower>(InstalledBuilding);
				if(CastTower)
				{
					UnderRampart->InstallTowerOnRampart(CastTower);				
				}				
			}
			else
				return false;
		}
	}	
	
	AddBuilding(InstalledBuilding);
	
	InstalledBuilding->StartBuild();
	
	BF_Weak_PreviewBuilding.Get()->Destroy();	
	BF_Weak_PreviewBuilding.Reset();
	return true;
}

void ACitadelBuildingFactory::AddBuilding(const TWeakObjectPtr<ACitadelBuilding> InBuilding)
{
	InstalledBuildingList.Add(InBuilding);
}

void ACitadelBuildingFactory::RemoveBuilding(const TWeakObjectPtr<ACitadelBuilding> InBuilding)
{
	InstalledBuildingList.Remove(InBuilding);
}

EBuildingType ACitadelBuildingFactory::GetSelectedBuilding() const
{
	return SelectedBuilding;
}
void ACitadelBuildingFactory::SelectBuilding(EBuildingType InBuildingType)
{
	SelectedBuilding = InBuildingType;
	const auto BuildingClass =
		GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData()->GetBuildingClass(InBuildingType);
}