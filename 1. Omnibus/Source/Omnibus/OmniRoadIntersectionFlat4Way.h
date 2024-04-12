// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniRoad.h"
#include "OmniRoadIntersectionFlat4Way.generated.h"

enum class ERoadApproach;
enum class ERoadDirection : uint8;
struct FIntersectionDimensionInfo;
/**
 * 4거리 평면 교차로
 * 회전용 차선 있는 버전.
 */
UCLASS()
class OMNIBUS_API AOmniRoadIntersectionFlat4Way : public AOmniRoad
{
	GENERATED_BODY()

public:
	AOmniRoadIntersectionFlat4Way();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void SetSplinesTransform();

	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void SetCompTransform();

	UFUNCTION()
	void SetLanePoints();

	/** 진입 위치에서 진행방향에 맞는 차선 반환 */
	USplineComponent* GetLaneByApproachIdx(const uint32 ApproachIdx, const uint8 LaneDirection) const;
	/** 진입 위치에서 진행방향에 맞는 차선 반환 */
	USplineComponent* GetLaneByApproachIdx(const uint32 ApproachIdx, const ERoadDirection& LaneDirection) const;
	/** 진입 위치에서 진행방향에 맞는 차선 반환 */
	USplineComponent* GetLaneByApproachIdx(const ERoadApproach& ApproachIdx, const ERoadDirection& LaneDirection) const;

	virtual double GetRoadWidth() override;

	/**
	 * 도로 크기, 치수 정보 가져오기
	 * @return FIntersectionDimensionInfo 구조체
	 */
	FIntersectionDimensionInfo GetIntersectionDimensionInfo();

	virtual USplineComponent* GetSplineToNextRoad(const int32 InLaneApproachIdx, AOmniRoad* InNextTargetRoad) override;
	virtual USplineComponent* GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad) override;

	virtual void AddConnectedRoadSingle(AOmniRoad* InRoad, const uint8 InAccessIdx) override;

private:
	ERoadDirection GetLaneDirectionByConnectedIdx(const uint32 StartLaneApproachIdx, const uint32 TargetRoadIdx) const;

	static uint32 ConvertDetectorIdxToConnectRoadIdx(const uint32 InDetectorIdx);
	static uint32 ConvertConnectRoadIdxToDetectorIdx(const uint32 InConnectRoadIdx);
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Flat4WayMesh;

	/** 좌상단부터 시계방향으로 차선 진출입로의 위치를 저장함. */
	TArray<FVector> LanePoints;
};
