// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniLineBusRoute.generated.h"

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

	void SpawnBus(const FTransform& SpawnTransform);
	
protected:
	/** 가장 가까운 2차선을 찾음. */
	void GetNearestOmniRoadTwoLaneAndLane(AOmniRoad*& OutNearRoad, uint32& OutLaneIdx) const;

	/** 버스 루트 생성. 현재 랜덤으로 생성함. */
	void GenerateRouteRoad();

	/** 버스 노선 스플라인에 경로(스플라인 차선) 추가*/
	void PushToRouteSpline(const USplineComponent* InAddLaneSpline);

	void MakeUTurnRouteSpline(const int32 InStartPoint, const int32 InEndPoint);

public:
	USplineComponent* GetRouteSpline() const { return RouteSpline; }

protected:
	TArray<TWeakObjectPtr<AOmniRoad>> BusRouteRoads;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* RouteSpline;

	TArray<TWeakObjectPtr<AOmniVehicleBus>> MyBuses;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AOmniRoad*> TempBusRouteRoads;

};
