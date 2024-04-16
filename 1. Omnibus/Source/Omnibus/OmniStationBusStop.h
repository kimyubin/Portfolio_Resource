// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniStationBusStop.generated.h"

class UBoxComponent;
class AOmniRoad;
/** 정류장 최상위 클래스*/
UCLASS()
class OMNIBUS_API AOmniStationBusStop : public AOmniActor
{
	GENERATED_BODY()
	
public:	
	AOmniStationBusStop();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	AOmniRoad* GetOwnerOmniRoad() const;
	void       SetOwnerOmniRoad(AOmniRoad* InOwnerOmniRoad);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UStaticMeshComponent* BusStopMesh;

	/** 버스 감속 구간*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UBoxComponent* DecelerationArea;

	/** 버스 정차 위치 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BusStop")
	UBoxComponent* BusStopArea;

private:
	TWeakObjectPtr<AOmniRoad> OwnerOmniRoad;
};
