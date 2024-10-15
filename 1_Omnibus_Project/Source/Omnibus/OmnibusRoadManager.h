// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "Containers/CircularQueue.h"
#include "OmnibusRoadManager.generated.h"

struct FSectorInfo;
class AOmnibusLevelManager;
class APathVisualizator;
struct FTransferPath;
class AOmniPassenger;
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

	/**
	 * 버스 노선이 변경되었음을 전파합니다.
	 * 환승 경로가 없는 승객들이 경로찾기를 다시 수행하도록 합니다.
	 * 다른 승객들은 정류장에서 내릴 때 다시 찾습니다.
	 */
	void NotifyUpdateBusRoute();

	/** 환승 경로 찾기에 실패한 승객들을 등록합니다. */
	void AddNoTransferPathPassenger(AOmniPassenger* InPassenger);
	
	/** 적절한 출발지와 목적지 반환 */
	std::tuple<FSectorInfo, TArray<FSectorInfo>> GetHomeAndDests();

	/** 전체 버스 노선도의 버전 번호 반환 */
	uint64 GetBusRouteMapVersion() const { return BusRouteMapVersion; };
	
	//~=============================================================================
	// PassengerPool 관련 함수

	/** PassengerPool을 채웁니다. 여러 tick에 나눠서 채웁니다. */
	void FillPassengerPool();

	/** Passenger를 비활성화된 상태로 스폰한 후, PassengerPool에 넣습니다. */
	void GeneratePassengerInPool();

	/** 비활성화된 Passenger를 Pool에서 가져온 후 활성화 시켜서 반환합니다. */
	AOmniPassenger* PopPassengerInPool();


	//~=============================================================================
	// 가시성, 렌더링 관련 함수

	/** 버스 노선 렌더링 가시성 일괄 변경 */
	void SetRoutesRender(const bool InVisibility);

	/** 버스 노선 렌더링 가시성 일괄 토글 */
	void ToggleRoutesRender();

	bool IsRouteVisible() const;

	/** 승객 가시성 일괄 변경 */
	void SetPassengerRender(const bool InVisibility);

	bool IsPassengerVisible() const { return bPassengerVisible; };
	
	/** 경로 시각화하기 */
	APathVisualizator* VisualizePath(const FTransferPath& InTransferPath);


	//~=============================================================================
	// 시스템 관리 함수.

	/**
	 * 관리 함수들을 순서에 맞게 일괄 호출하는 함수.
	 * 도로, 정류장 등이 주변을 탐색하고, 데이터를 정리하는 함수들을 호출합니다.
	 * 에디터에서도 호출가능.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorAuto)
	void ExecuteAutoManageFunction();

	
	//~=============================================================================
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

	
	//~=============================================================================
	// 정류장

	/** 모든 정류장 획득 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForStation)
	void CollectBusStop();

	/**
	 * 보유한 정류장의 감지,정리 함수 호출. 도로/CityBlock 대상.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForStation)
	void ExecuteAllStationsDetect();

	
	//~=============================================================================
	// CityBlock

	/** 모든 CityBlock 획득 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForCityBlock)
	void CollectCityBlock();

	/** 블록이 가진 유효하지 않은 버스 정류장 정리 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForCityBlock)
	void CleanupInvalidStopInCityBlock();

	//~=============================================================================
	// Route
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForRoute)
	void CollectRoute();


	//~=============================================================================
	// Passenger
	
	/** 모든 승객 획득 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = EditorForCityBlock)
	void CollectPassenger();


	
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

	void AddPassenger(AOmniPassenger* InPassenger);	
	void RemovePassengerByID(const uint64 InId);
	void RemovePassenger(AOmniLineBusRoute* InPassenger);
	AOmniPassenger* FindPassenger(const uint64 InId);

protected:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniRoad>> OmniRoadsTMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniStationBusStop>> OmniStationTMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniCityBlock>> OmniCityBlockList;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniLineBusRoute>> OmniRouteTMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<uint64, TWeakObjectPtr<AOmniPassenger>> OmniPassengerTMap;

	/** 환승경로 없는 승객 목록*/
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TSet<TWeakObjectPtr<AOmniPassenger>> NoTransferPathPassengerList;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<APathVisualizator>> PathVisualizatorCacheList;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	AOmnibusLevelManager* LevelManager;


	//~=============================================================================
	// PassengerPool용
	/** 사전 생성된 승객 풀 */
	TUniquePtr<TCircularQueue<TWeakObjectPtr<AOmniPassenger>>> PassengerPool;

	/** 승객풀 최대값. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	uint32 PoolCapacity;

	/** 승객풀 하한값. 이 값도다 적으면 최대값까지 추가로 생산해 냅니다. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	uint32 PoolLowerThreshold;

	/** 객체풀이 부족하다. */
	bool bPoolInsufficient;


	/**
	 * 전체 버스 노선도의 버전 번호.
	 * 노선이 추가, 삭제, 수정될 경우 1씩 증가하여, 노선도에 변경사항이 있음을 알리는 역할을 합니다.  
	 */
	uint64 BusRouteMapVersion;


	/** 승객 가시성 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bPassengerVisible;
	
	/** 버스 노선의 가시성 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bRouteVisible;

	/** 시각화 액터 캐시 사이즈 */
	static constexpr uint64 VisualizatorCacheSize = 10;
};
