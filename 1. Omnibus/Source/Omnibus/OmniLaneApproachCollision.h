// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "OmniLaneApproachCollision.generated.h"

class USplineComponent;
class AOmniRoad;
/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmniLaneApproachCollision : public UBoxComponent
{
	GENERATED_BODY()

public:
	UOmniLaneApproachCollision();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ApproachBeginOverlap(UPrimitiveComponent* OverlappedComp
	                        , AActor*              OtherActor
	                        , UPrimitiveComponent* OtherComp
	                        , int32                OtherBodyIndex
	                        , bool                 bFromSweep
	                        , const FHitResult&    SweepResult);

	void InitLaneApproach(AOmniRoad* InOwnerOmniRoad, const int32 InApproachIdx);

	USplineComponent* GetNextSplineByRoad(AOmniRoad* InNextTargetRoad) const;

private:
	TWeakObjectPtr<AOmniRoad> OwnerOmniRoad;
	int32 LaneApproachIdx;   // OmniRoad의 배열에서 본 컴포넌트의 Index. 음수는 초기화 미실시한 것
};
