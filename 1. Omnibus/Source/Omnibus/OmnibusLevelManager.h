// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmnibusLevelManager.generated.h"

class UOmniTimeManager;

/**
 * 승객 스폰, 지역 개방 등 레벨에서 발생하는 이벤트 관리
 */
UCLASS()
class OMNIBUS_API AOmnibusLevelManager : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmnibusLevelManager();
	void Initialize(AOmnibusRoadManager* InRoadManager);
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	void GeneratePassenger();
	void OpenCityBlock();
	void ChangeCityBlockType();

	//~=============================================================================
	// 각 이벤트 발생 간격을 반환합니다.(초)
	float GetIntervalSec_GeneratePassenger() const;
	float GetIntervalSec_OpenCityBlock() const;
	float GetIntervalSec_ChangeCityBlockType() const;

	UOmniTimeManager* GetOmniTimerManager() const;

protected:
	TWeakObjectPtr<AOmnibusRoadManager> RoadManager;

	//~=============================================================================
	// 타이머 핸들
	FTimerHandle PassengerGenerateHandle;
	FTimerHandle BlockOpenHandle;
	FTimerHandle BlockTypeChangeHandle;
};
