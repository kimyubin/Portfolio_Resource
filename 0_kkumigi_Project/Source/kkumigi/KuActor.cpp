// Fill out your copyright notice in the Description page of Project Settings.


#include "KuActor.h"

#include "KuGameInstance.h"


AKuActor::AKuActor()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AKuActor::BeginPlay()
{
	Super::BeginPlay();
}


void AKuActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UKuGameInstance* AKuActor::GetKuGameInstance() const
{
	return GetGameInstance<UKuGameInstance>();
}

UKuPlayData* AKuActor::GetKuPlayData() const
{
	return GetKuGameInstance()->GetKuPlayData();
}

UKuSaveManager* AKuActor::GetKuSaveManager() const
{
	return GetKuGameInstance()->GetKuSaveManager();
}

UItemManager* AKuActor::GetItemManager() const
{
	return GetKuGameInstance()->GetItemManager();
}

UKuUIManager* AKuActor::GetKuUIManager() const
{
	return GetKuGameInstance()->GetUIManager();
}

UChunkManager* AKuActor::GetChunkManager() const
{
	return GetKuGameInstance()->GetChunkManager();
}
