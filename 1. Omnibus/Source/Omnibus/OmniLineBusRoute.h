// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniLineBusRoute.generated.h"

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

public:
	virtual void Tick(float DeltaTime) override;

	/**
	 * 노선 및 버스 생성.
	 * 스폰 직후 수동으로 발동, 혹은 로드 매니저가 도로를 전부 수집 및 상호 연결 시킨 이후 발동.
	 * @see AOmnibusRoadManager::PostBeginPlay()
	 */
	void MakeRouteAndBus();
	void SpawnBus(const FTransform& SpawnTransform);
	
protected:
	/** 가장 가까운 2차선을 찾음. */
	void GetNearestOmniRoadTwoLaneAndLane(AOmniRoad*& OutNearRoad, uint32& OutLaneIdx) const;

	/** 버스 노선에 들어갈 도로 생성. 현재 랜덤으로 생성함. */
	void GenerateRoute();

	/** 버스 노선을 바탕으로 경로 스플라인 생성. */
	void MakeRouteSpline();

	/** 노선 스플라인을 시각화하기 위한 스플라인 메시 생성 */
	void MakeSplineMeshComponents();

	/** 버스 노선 스플라인에 경로(스플라인 차선) 추가*/
	void PushToRouteSpline(const USplineComponent* InAddLaneSpline);

	void MakeUTurnRouteSpline(const int32 InStartPoint, const int32 InEndPoint);

public:
	USplineComponent* GetRouteSpline() const { return RouteSpline; }

	void InsertBusStop(AOmniStationBusStop* InFrontBusStop, AOmniStationBusStop* InAddBusStop);

	void SetRouteRender(const bool SetVisibility);
	void ToggleRouteRender();
	
protected:
	/** BusRouteRoads를 바탕으로 만든, 실제 버스가 따라다니는 노선*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* RouteSpline;

	/** 노선 시각화용 메시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PlacedMesh;

	/** 버스 정류장 순서대로 보관 */
	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniStationBusStop>> RouteBusStops;

	/** 각 버스 정류장 사이를 이어주는 도로 */
	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniRoad>> BusRouteRoads;

	UPROPERTY(VisibleAnywhere, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniVehicleBus>> MyBuses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BusRoute, meta = (AllowPrivateAccess = "true"))
	FColor LineColor;
};
