// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniRoad.h"
#include "GameFramework/Actor.h"
#include "OmniRoadTwoLane.generated.h"


struct FRoadLengthMesh;
/**
 * 기본 왕복 2차선 도로. 
 */
UCLASS()
class OMNIBUS_API AOmniRoadTwoLane : public AOmniRoad
{
	GENERATED_BODY()

public:
	AOmniRoadTwoLane();
#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
#endif // WITH_EDITOR

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	/**
	 * 이웃하는 도로 감지 및 추가.
	 */
	virtual void DetectAllConnectedOmniRoad() override;

	/** RoadSpline의 시작과 끝 위치를 특정 간격으로 스냅하기. z축 0.0으로 고정. 말단위치*/
	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	void SnapRoadSplineTerminalLocation();

	/////////////////////////////////////////////////////////////////////
	// Construction Script 전용 함수 시작.

	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void MakeSplineMeshComponentAlongSpline();

	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void UpdateLaneSplinesAlongRoadCenter();

	// Construction Script 전용 함수 끝.
	/////////////////////////////////////////////////////////////////////

	virtual AOmniRoad* GetNextRoadByLaneIdx(const int32 InLaneIdx) override;

	virtual int32 FindLaneIdxToNextRoad(AOmniRoad* InPrevRoad
	                                  , AOmniRoad* InNextTargetRoad) override;

	UFUNCTION()
	USplineComponent* GetMainRoadSpline() const;

	/** 도로 길이에 따른 메시 반환 */
	UFUNCTION()
	UStaticMesh* GetRoadMesh(float InLaneLength);

	virtual bool CanInstallBusStop() const override { return true; }

	/**
	 * 적용할 스태틱 메시. 휘어져야하기 때문에 버텍스가 충분해야합니다.
	 * 스플라인 포인트 사이의 거리에 따라 적용되는 메시의 버텍스를 차등하기 위해 서로 다른 수준의 LOD를 가진 Mesh들로 구성됩니다.
	 * 길이가 길면 더 조밀한 메시를 사용합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FRoadLengthMesh> LaneMeshList;
};
