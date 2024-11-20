// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniRoad.h"
#include "OmniRoad4Way.generated.h"

enum class ERoadApproach;
enum class ERoadDirection : uint8;
struct FIntersectionDimensionInfo;
/**
 * 4거리 평면 교차로
 * 회전용 차선 있는 버전.
 */
UCLASS()
class OMNIBUS_API AOmniRoad4Way : public AOmniRoad
{
	GENERATED_BODY()

public:
	AOmniRoad4Way();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void SetSplinesTransform();

	UFUNCTION()
	void SetLanePoints();

	/** 진입 위치에서 진행방향에 맞는 차선 번호 계산 */
	int32 CalculateLaneIdx(const int32 ApproachIdx, const uint8 LaneDirection) const;
	/** 진입 위치에서 진행방향에 맞는 차선 반환 */
	USplineComponent* GetLaneByApproachIdx(const int32 ApproachIdx, const uint8 LaneDirection) const;
	/** 진입 위치에서 진행방향에 맞는 차선 반환 */
	USplineComponent* GetLaneByApproachIdx(const int32 ApproachIdx, const ERoadDirection& LaneDirection) const;
	/** 진입 위치에서 진행방향에 맞는 차선 반환 */
	USplineComponent* GetLaneByApproachIdx(const ERoadApproach& ApproachIdx, const ERoadDirection& LaneDirection) const;

	virtual double GetRoadWidth() override;

	/**
	 * 도로 크기, 치수 정보 가져오기
	 * @return FIntersectionDimensionInfo 구조체
	 */
	FIntersectionDimensionInfo GetIntersectionDimensionInfo();

	virtual AOmniRoad* GetNextRoadByLaneIdx(const int32 InLaneIdx) override;
	
	virtual int32 FindLaneIdxToNextRoad(AOmniRoad* InPrevRoad
	                                  , AOmniRoad* InNextTargetRoad) override;

	virtual void AddConnectedRoadSingle(AOmniRoad* InRoad, const int32 InAccessIdx) override;

private:
	ERoadDirection GetLaneDirectionByConnectedIdx(const int32 StartLaneApproachIdx, const int32 TargetRoadIdx) const;

	/**
	 * ConnectorIdx를 바탕으로 Connector가 있어야할 위치를 Idx 형식으로 반환.
	 * @param InConnectorIdx 찾고자하는 Connector의 index
	 * @return 0 : OutRoadIdx - 해당 Connector의 부모 도로
	 * @return 1 : OutSplinePointIdx - 해당 Connector가 부모 도로에서 있어야할 위치. SplinePointIndex
	 */
	static std::tuple<int32, int32> GetConnectorPositionIdx(const int32 InConnectorIdx);
	
public:
	virtual bool CanInstallBusStop() const override { return false; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Flat4WayMesh;

	/** 좌상단부터 시계방향으로 차선 진출입로의 위치를 저장함. */
	TArray<FVector> LanePoints;

	/** 좌우 방향 차선 스플라인의 접선의 크기를 지정합니다. 값이 작을 수록 직선에 가까워집니다. */
	UPROPERTY(EditAnywhere)
	double TangentSizeRate;
};
