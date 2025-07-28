// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniLineBusRoute.generated.h"

enum class ECarType : uint8;
struct FBusStopDistance;
class AOmniCityBlock;
struct FLaneInfo;
class AOmniStationBusStop;
class AOmniVehicleBus;
class USplineComponent;
class AOmniRoad;

/** 각 버스가 지나가는 버스 노선의 최상위 클래스. 버스와 정류장을 보유함.*/
UCLASS()
class OMNIBUS_API AOmniLineBusRoute : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmniLineBusRoute();

protected:
	virtual void BeginPlay() override;
	virtual void PostBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	//~=============================================================================
	// FLaneInfo 기반 경로 찾기 함수군.

	/**
	 * 정류장 사이의 경로와 찾습니다.
	 * 2개 이상의 정류장을 순서대로 경유하는 경로를 찾습니다.
	 * 처음과 끝이 동일하다면 순환선을 만들고,
	 * 그렇지 않다면 양 끝에서 U턴하는 경로를 만듭니다.
	 * 
	 * @param InBusStops 
	 * @return 
	 */
	static TArray<FLaneInfo> FindLanePathByWayPoints(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InBusStops);

	/**
	 * 두 정류장 사이의 최단 거리 차선 정보를 찾아서 반환.
	 * 기존 경로에서 정류장을 추가하는 기능도 겸하기 위해, LaneInfoRoute를 직접 수정하지 않음.
	 * 
	 * @param InStartBusStop 
	 * @param InEndBusStop 
	 * @return  
	 */
	static TArray<FLaneInfo> FindLanePath(const AOmniStationBusStop* InStartBusStop, const AOmniStationBusStop* InEndBusStop);

	/**
	 * FLaneInfo에 있는 OmniRoad'만' 사용하여, 차선 경로(TArray<FLaneInfo>) 재구성합니다.
	 * 도로 정보만을 사용하기 때문에 역방향 계산을 할 수 있습니다.
	 * 
	 * @param InRouteRoads 사용할 도로 정보를 포함하는 FLaneInfo 배열
	 * @return OmniRoad를 통해 재구성한 차선 경로
	 */
	static TArray<FLaneInfo> ReconstructLanePathByRoads(const TArray<FLaneInfo>& InRouteRoads);


	//~=============================================================================
	// 노선 생성 및 버스 스폰
	/**
	 * 노선 정보 초기화 및 경유 정류장 목록을 바탕으로 노선을 구축합니다.
	 * 
	 * @param InWayPointList 경유 정류장 목록
	 */
	void InitializeRoute(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InWayPointList);

	/**
	 * 경유 정류장을 잇는 노선 생성 및 버스 생성.
	 * 스폰 직후 수동으로 발동.
	 * 
	 * @param InWayPointList 경유 정류장 목록
	 * @see AOmnibusPlayerController::LeftButton()
	 */
	void InitRoutePathAndBus(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InWayPointList);

	/**
	 * 버스 정류장 사이에 버스정류장을 추가합니다.
	 * 추가되는 정류장을 거쳐서 가는 경로를 찾아, 버스 노선(경로, 정류장 등) 정보를 업데이트합니다.
	 * 스플라인 메시와 RouteSpline도 모두 갱신합니다.
	 * 
	 * @param InFrontWayPointIdx 추가되는 전방(선행) 버스 정류장
	 * @param InAddBusStop 추가할 버스 정류장.
	 */
	void InsertWayPointInRoute(const int32 InFrontWayPointIdx, AOmniStationBusStop* InAddBusStop);


	/**
	 * 노선 위에 버스를 스폰하고, 적절한 초기화를 진행합니다.
	 * 
	 * @param InSpawnCarType 스폰한 버스의 종류입니다.
	 * @param InSpawnRouteDist 스폰할 버스의 위치를 노선 스플라인의 Dist로 지정합니다.
	 */
	void SpawnBusOnRoute(ECarType InSpawnCarType, const float InSpawnRouteDist);

protected:
	/** 임의 위치에 버스를 스폰하고 등록합니다. */
	void SpawnBusByTransform(const FTransform& SpawnTransform, ECarType InSpawnCarType, const float InSpawnRouteDist);



	//~=============================================================================
	// 노선 관련 컴포넌트 구축

	/** 노선 정보(FLaneInfo)를 바탕으로 노선 스플라인 구성. */
	void UpdateRouteSpline();

	/**
	 * 경로(스플라인 차선) 정보를 노선 스플라인 컴포넌트에 반영하고, 버스 정류장 목록을 업데이트 합니다.
	 * 
	 * @param InLaneInfo 추가할 경로 정보(도로와 차선 번호)
	 */
	void PushLaneAndBusStops(const FLaneInfo& InLaneInfo);

	/**
	 * 1. 리스트에 버스 정류장 추가.
	 * 2. 버스 정류장 Dist에 노선 길이를 더함.
	 * 3. 버스 정류장에 노선 추가.
	 * 
	 * @param InOutAddBusStopList 추가할 버스 정류장 목록 
	 * @param InAddedLaneLength 정류장 목록이 추가될 때, 같이 추가된 차선의 길이
	 */
	void AppendBusStopDist(TArray<FBusStopDistance>& InOutAddBusStopList, const float InAddedLaneLength);


	/** 지정된 스플라인 포인트 사이에 유턴 스플라인을 만듭니다. */
	void MakeUTurnRouteSpline(const int32 InStartPoint, const int32 InEndPoint);

	/** RouteSpline을 따라 노선 시각화용 스플라인 메시를 생성합니다. */
	void CreateSplineMeshComponents();

public:
	USplineComponent*        GetRouteSpline() const;
	float                    GetRouteLength() const;
	TArray<FBusStopDistance> GetBusStopDistanceList() const;
	FBusStopDistance         GetBusStopDist(int32 InIdx) const;
	FLinearColor             GetLineColor() const;
	
	/** BusStopDistanceList에서 @InBusStop이 있는 모든 요소의 인덱스를 찾아 반환합니다. */
	TArray<int32> FindBusStopIdxList(AOmniStationBusStop* InBusStop) const;

	/** 인덱스로 두 정류장 사이의 최단 거리를 구합니다. */
	float         GetShortPathLength(const int32 InStartBusIdx, const int32 InEndBusIdx);

	/** @InDist에서 가장 가까운 다음 정류장(ThisStop)을 찾습니다. 0 ~ (ArrayMax-1)를 순환합니다. */
	int32         FindThisStopIdxByDist(const float InDist);

	/** 다음 버스 정류장 인덱스를 반환합니다. 0 ~ (ArrayMax-1)를 순환합니다. */
	int32         GetNextBusStopIdx(const int32 InCurrentIdx) const;


	void UnlinkBusStop(AOmniStationBusStop* InBusStop);
	void ClearBusStopList();


	/** 버스 노선 렌더링 순서를 반환합니다. */
	static float GetRenderOrderOffset();

	/** 버스 노선 가시성 설정. */
	void SetRouteRender(const bool InVisibility);
	void ToggleRouteRender();


	/**
	 * 두 버스 정류장 사이을 잇는 가장 가까운 경로 찾고,
	 * 경로 길이와 출발 정류장의 인덱스를 반환합니다.
	 * 
	 * @param InBusStopDistanceList   버스 노선을 구성하는 FBusStopDistance 배열
	 * @param InBusLineLength         버스 노선 전체 길이
	 * @param InStartBusStop          출발 정류장의 약포인터
	 * @param InEndBusStopDistanceIdx 도착 정류장의 Idx
	 * 
	 * @return 0 : int32 - 도착 정류장과 가장 가까운 출발 정류장의 Idx.
	 * @return 1 : float - 두 정류장의 최단 거리.
	 */
	static std::tuple<int32, float> GetShortestStartIdxAndDist(const TArray<FBusStopDistance>& InBusStopDistanceList
	                                                         , const float InBusLineLength
	                                                         , const TWeakObjectPtr<AOmniStationBusStop>& InStartBusStop
	                                                         , const int32& InEndBusStopDistanceIdx);


	/** 무작위 노선 생성. 레벨에 스폰되어져 있는 노선 대상. */
	void GenerateRandomRoute();

protected:
	/** LaneInfoRoute를 바탕으로 만든, 실제 버스가 따라다니는 노선*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* RouteSpline;

	/**
	 * 노선 생성용 주요 경유 정류장 목록입니다.
	 * 이 목록을 통해 노선을 만들 수 있습니다.
	 * 처음과 끝이 같다면 순환선을 만듭니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniStationBusStop>> WayPointStopList;

	/**
	 * 버스 정류장 순서대로 보관
	 * 버스노선 Spline의 Distance, 버스 정류장 약포인터
	 */
	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<FBusStopDistance> BusStopDistanceList;

	/** 각 버스 정류장을 이어주는 차선 정보. 실질적인 노선. */
	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<FLaneInfo> LaneInfoRoute;

	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniVehicleBus>> MyBuses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	FLinearColor LineColor;
};
