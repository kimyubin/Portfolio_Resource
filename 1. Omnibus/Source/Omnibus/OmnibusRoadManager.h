// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmnibusRoadManager.generated.h"

class AOmniRoad;
/**
 * 도로망 관리
 */
UCLASS()
class OMNIBUS_API AOmnibusRoadManager : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmnibusRoadManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	
	void         CollectOmniRoad();
	void         AddOmniRoad(AOmniRoad* InRoad);
	void         RemoveOmniRoad(const uint64 InId);
	void         RemoveOmniRoad(AOmniRoad* InRoad);
	AOmniRoad*   FindOmniRoad(const uint64 InId);

protected:
	UPROPERTY()
	TMap<uint64, AOmniRoad*> OmniRoadsTMap;
};
