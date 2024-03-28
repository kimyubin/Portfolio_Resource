// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelEnemyFactory.h"

#include "CitadelBuilding.h"
#include "CitadelStatics.h"

ACitadelEnemyFactory::ACitadelEnemyFactory()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACitadelEnemyFactory::BeginPlay()
{
	Super::BeginPlay();	
}

void ACitadelEnemyFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ACitadelEnemyFactory::SpawnEnemy()
{
	//EnemyBuildingList.Add
}

void ACitadelEnemyFactory::SearchNearEnemyList(TWeakObjectPtr<ACitadelBuilding> InBuilding, TArray<TWeakObjectPtr<class ACitadelBuilding>>& Results)
{
	FVector InLocation = InBuilding->GetActorLocation();
	int32 InAtkRange = InBuilding->GetAttackRange();
	
	
	for (auto& MyEnemy : EnemyBuildingList)
	{
		if(MyEnemy.IsValid())
		{
			FVector EnemyLocation = MyEnemy->GetActorLocation();
			double Distance = UCitadelStatics::CalPlaneDistance(InLocation,EnemyLocation);
			if(Distance <= InAtkRange)
				Results.Add(MyEnemy);
		}
	}	
}

