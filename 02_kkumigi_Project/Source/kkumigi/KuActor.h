// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KuActor.generated.h"

class UChunkManager;
class UKuUIManager;
class UItemManager;
class UKuSaveManager;
class UKuPlayData;
class UKuGameInstance;

UCLASS()
class KKUMIGI_API AKuActor : public AActor
{
	GENERATED_BODY()

public:
	AKuActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	/** 게임 인스턴스 및 시스템 객체 */

	UKuGameInstance* GetKuGameInstance() const;

	UKuPlayData* GetKuPlayData() const;
	UKuSaveManager* GetKuSaveManager() const;
	UItemManager* GetItemManager() const;
	UKuUIManager* GetKuUIManager() const;
	UChunkManager* GetChunkManager() const;
};
