// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniStationBusStop.generated.h"

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
	bool DetectRoadAndLane(UClass* InClassFilter, AOmniRoad*& OutNearRoad, uint32& OutNearLaneIdx) const;

	UFUNCTION(Category = PostEditMove)
	bool DetectCityBlock(UClass* InClassFilter, AOmniCityBlock*& OutCityBlock) const;


	AOmniRoad* GetOwnerOmniRoad() const;
	void       UpdateOwnerOmniRoad(AOmniRoad* InOwnerOmniRoad);
	void       UnlinkOwnerOmniRoad();
	uint32     GetOwnerLaneIdx() const { return OwnerLaneIndex; };
	FVector    GetClosestLanePointLocation() const { return ClosestLanePointLocation; };

	AOmniCityBlock* GetOwnerOmniCityBlock() const;
	void            UpdateOwnerOmniCityBlock(AOmniCityBlock* InOwnerOmniCityBlock);
	void            UnlinkOwnerOmniCityBlock();


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UStaticMeshComponent* BusStopMesh;

	/** 버스 정류장 위치 감지 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UBoxComponent* StationDetector;

	/** 버스 감속 구간*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UBoxComponent* DecelerationArea;

	/** 버스 정차 위치 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UBoxComponent* BusStopArea;

protected:
	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	TWeakObjectPtr<AOmniRoad> OwnerOmniRoad;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	uint32 OwnerLaneIndex;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	float ClosestLaneInputKey;

	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	FVector ClosestLanePointLocation;
	
	UPROPERTY(VisibleAnywhere, Category = "BusStop")
	TWeakObjectPtr<AOmniCityBlock> OwnerOmniCityBlock;
};
