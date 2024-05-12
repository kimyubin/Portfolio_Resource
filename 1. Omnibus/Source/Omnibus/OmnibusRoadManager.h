// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmnibusRoadManager.generated.h"

class AOmniLineBusRoute;
class AOmniCityBlock;
class AOmniStationBusStop;
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
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void PostBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	
	// ~===============
	// 시스템 관리 함수.

	/**
	 * 관리 함수들을 순서에 맞게 일괄 호출하는 함수.
	 * 도로, 정류장 등이 주변을 탐색하고, 데이터를 정리하는 함수들을 호출함.
	 * 에디터에서도 호출가능.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorAuto)
	void ExecuteAutoManageFunction();

	
	// ~===============
	// 도로

	/** 모든 도로 획득 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForRoad)
	void CollectOmniRoad();

	/** 보유한 도로 간 연결 함수 호출. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForRoad)
	void ExecuteAllRoadsDetect();

	/** 도로가 가진 유효하지 않은 버스 정류장 정리 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForRoad)
	void CleanupInvalidStationInRoad();

	
	// ~===============
	// 정류장

	/** 모든 정류장 획득 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForStation)
	void CollectStation();

	/**
	 * 보유한 정류장의 감지,정리 함수 호출. 도로/CityBlock 대상.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForStation)
	void ExecuteAllStationsDetect();

	
	// ~===============
	// CityBlock

	/** 모든 CityBlock 획득 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForCityBlock)
	void CollectCityBlock();

	/** 블록이 가진 유효하지 않은 버스 정류장 정리 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForCityBlock)
	void CleanupInvalidStationInCityBlock();

	// ~===============
	// Route
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForRoute)
	void CollectRoute();
	
	void AddOmniRoad(AOmniRoad* InRoad);
	void RemoveOmniRoadByID(const uint64 InId);
	void RemoveOmniRoad(AOmniRoad* InRoad);
	AOmniRoad* FindOmniRoad(const uint64 InId);

	void AddStation(AOmniStationBusStop* InStation);
	void RemoveStationByID(const uint64 InId);
	void RemoveStation(AOmniStationBusStop* InStation);
	AOmniStationBusStop* FindStation(const uint64 InId);

	void AddCityBlock(AOmniCityBlock* InCityBlock);
	void RemoveCityBlockByID(const uint64 InId);
	void RemoveCityBlock(AOmniCityBlock* InCityBlock);
	AOmniCityBlock* FindCityBlock(const uint64 InId);

	void AddOmniRoute(AOmniLineBusRoute* InOmniRoute);
	void RemoveOmniRouteByID(const uint64 InId);
	void RemoveOmniRoute(AOmniLineBusRoute* InOmniRoute);
	AOmniLineBusRoute* FindOmniRoute(const uint64 InId);

	// ~===============
	/** 버스 노선 렌더링 가시성 일괄 변경 */
	void SetRoutesRender(const bool SetVisibility);
	/** 버스 노선 렌더링 가시성 일괄 토글 */
	void ToggleRoutesRender();

	bool IsRouteVisible() const;

protected:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniRoad>> OmniRoadsTMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniStationBusStop>> OmniStationTMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniCityBlock>> OmniCityBlockTMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniLineBusRoute>> OmniRouteTMap;

	/** 버스 노선의 가시성 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bRouteVisible;
};
