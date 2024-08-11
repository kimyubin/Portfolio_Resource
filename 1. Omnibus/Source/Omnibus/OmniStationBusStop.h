// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniStationBusStop.generated.h"

class AOmniPassenger;
class AOmniVehicleBus;
class USplineComponent;
class AOmniLineBusRoute;
class USphereComponent;
class AOmniCityBlock;
class UBoxComponent;
class AOmniRoad;

/** 정류장 최상위 클래스*/
UCLASS()
class OMNIBUS_API AOmniStationBusStop : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmniStationBusStop();
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
#endif // WITH_EDITOR

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	void SearchRoadAndBlock();

	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	void SearchRoad();

	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	void SearchCityBlock();

	UFUNCTION(Category = PostEditMove)
	bool DetectRoadAndLane(UClass* InClassFilter, AOmniRoad*& OutNearRoad, int32& OutNearLaneIdx) const;

	UFUNCTION(Category = PostEditMove)
	bool DetectCityBlock(UClass* InClassFilter, AOmniCityBlock*& OutCityBlock) const;

	AOmniRoad* GetOwnerOmniRoad() const;
	void       UpdateOwnerOmniRoad(AOmniRoad* InOwnerOmniRoad);
	void       UnlinkOwnerOmniRoad();
	int32      GetOwnerLaneIdx() const { return OwnerLaneIndex; }
	float      GetClosestLaneInputKey() const { return ClosestLaneInputKey; }
	FVector    GetClosestLanePointLocation() const { return ClosestLanePointLocation; }

	AOmniCityBlock* GetOwnerOmniCityBlock() const;
	void            UpdateOwnerOmniCityBlock(AOmniCityBlock* InOwnerOmniCityBlock);
	void            UnlinkOwnerOmniCityBlock();

	void AddBusRoute(AOmniLineBusRoute* InRoute);
	void RemoveBusRoute(AOmniLineBusRoute* InRoute);
	TArray<TWeakObjectPtr<AOmniLineBusRoute>> GetBusRouteList() const { return BusRouteList; }

	/**
	 * 같은 도로를 공유하는 모든 정류장을 가져옵니다. 이웃한 정류장과 자기 자신을 포함한 정류장 배열을 가져옵니다.
	 * 맞은편 정류장일 수도 있고, 바로 옆 정류장일 수도 있습니다.
	 */
	TArray<TWeakObjectPtr<AOmniStationBusStop>> GetNeighborStops() const;
	
	/** 대기자들이 버스 승차를 시작합니다. */
	void StartEntryToBus(AOmniVehicleBus* InBus);

	/** 하차 혹은 주변 블록에서 정류장에 와서 대기합니다. */
	FTransform WaitAtStopLine(AOmniPassenger* InPassenger);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UStaticMeshComponent* BusStopMesh;

	/** 마우스 클릭, 터치 등 감지 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UStaticMeshComponent* InputMeshComponent;
	
	/** 버스 정류장 위치 감지 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UBoxComponent* StationDetector;

protected:
	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	TWeakObjectPtr<AOmniRoad> OwnerOmniRoad;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	int32 OwnerLaneIndex;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	float ClosestLaneInputKey;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	FVector ClosestLanePointLocation;
	
	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	TWeakObjectPtr<AOmniCityBlock> OwnerOmniCityBlock;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	TArray<TWeakObjectPtr<AOmniLineBusRoute>> BusRouteList;

	/** 대기자 명단 */
	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	TArray<TWeakObjectPtr<AOmniPassenger>> WaitPassengerList;

};
