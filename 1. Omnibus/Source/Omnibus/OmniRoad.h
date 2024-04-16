// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmniRoad.generated.h"

class UOmniDetectSphereComponent;
class AOmniStationBusStop;
class USplineComponent;
/**
 * 도로 최상위 객체
 */
UCLASS()
class OMNIBUS_API AOmniRoad : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmniRoad();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void BeginDestroy() override;

	/**
	 * 이웃하는 도로 감지 및 추가.
	 */
	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	virtual void DetectAllConnectedOmniRoad();

	/** 액터 위치 스냅하기. z축 0.0으로 고정. */
	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	void SnapActorLocation();

protected:
	/**
	 * 컴포넌트 개수를 결정하는 변수 초기화 및 다음 TArray를 해당 개수만큼 초기화.
	 * @param InRoadSplineNum : 도로 스플라인 개수. RoadSplines
	 * @param InRoadConnectDetectorNum : 도로 연결 감지 컴포넌트 개수. RoadConnectDetectors, ConnectedRoadsArray
	 * @param InLaneSplineNum : 차선 개수. LaneSplines
	 */
	void InitArrays(const uint8 InRoadSplineNum
	              , const uint8 InRoadConnectDetectorNum
	              , const uint8 InLaneSplineNum);

	void InitRoadSpline(const uint32 InIdx);
	void InitRoadConnectDetector(const uint32 InIdx, USplineComponent* InOwnerRoadSpline, const uint32 InSplinePointIdx);
	void InitLaneSpline(const uint32 InIdx, USplineComponent* InOwnerRoadSpline);

public:
	/**
	 * 이전 도로와 다음 도로를 이어주는 자신의 차선 반환.
	 * @param InPrevRoad 접근한 도로.
	 * @param InNextTargetRoad 다음 이동해야할 도로
	 * @return 다음 차선 lane
	 */
	virtual USplineComponent*   GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad);
	
	USplineComponent*           GetRoadSpline(const uint32 InIdx) const;
	USplineComponent*           GetLaneSpline(const uint32 InIdx) const;

	virtual void AddConnectedRoadSingle(AOmniRoad* InRoad, const uint8 InAccessIdx);
	void AddConnectedRoadBoth(const UOmniDetectSphereComponent* InTargetDetector, const uint8 InMyAccessIdx);

protected:
	void RemoveConnectedRoadSingle(const uint8 InAccessIdx);
	void RemoveConnectedRoadSingle(AOmniRoad* InRoad);
	void RemoveConnectedRoadBoth(const uint8 InAccessIdx);
	void RemoveConnectedRoadBoth(AOmniRoad* InRoad);
	/** 모든 연결 도로 양방향 제거. */
	void RemoveAllConnectedRoadsBoth();

	/** 도로 포인터로 연결된 도로 Idx 가져오기. 실패하면 -1 반환. */
	int32 FindConnectedRoadIdx(AOmniRoad* InRoad) const;

public:
	bool HasOwnedBusStop(const uint64 InId);
	void AddOwnedBusStop(AOmniStationBusStop* InBusStop);
	
	void RemoveOwnedBusStop(const uint64 InId);
	void RemoveOwnedBusStop(AOmniStationBusStop* InBusStop);

	/** ID로 버스정류장 찾아서 반환. 실패하면 nullptr 반환. */
	AOmniStationBusStop* FindOwnedBusStop(const uint64 InId);

	void FindPath(AOmniRoad* InStartRoad, AOmniRoad* InEndRoad, AOmniRoad* PrevRoad, TArray<AOmniRoad*>& OutPath);

	/** 테스트용. 임시. 이웃한 임의의 도로를 반환 */
	AOmniRoad* GetRandomConnectedRoad(AOmniRoad* PrevRoad);

	/**
	 * 다음으로 진행할 도로 반환
	 * @param InPrevRoad     직전 도로(진입한 도로)
	 * @return 다음 도로 (현재 차선을 타고 진출할 도로) 
	 */
	AOmniRoad* GetRandomNextRoad(AOmniRoad* InPrevRoad);

	AOmniRoad* GetConnectedRoad(const uint32 ConnectedRoadIdx) const { return ConnectedRoadsArray[ConnectedRoadIdx]; }
	
	uint8 GetLaneSplineNum() const { return LaneSplineNum; };
	
protected:
	/** 현재 사용하는 메쉬를 기반으로 도로폭 계산 */
	virtual double GetRoadWidth();

	/**
	 * 오버랩된 감지 컴포넌트 탐지 및 도로 연결 상태 갱신
	 * @param Detector 탐지할 컴포넌트
	 * @return 탐지된 컴포넌트, 없다면 nullptr
	 */
	UOmniDetectSphereComponent* DetectOmniRoad(UOmniDetectSphereComponent* const Detector);

protected:
	/** 연결되어있는 도로 집합 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<AOmniRoad*> ConnectedRoadsArray;

	/** 도로를 구성하는 기준 스플라인. 도로 중앙.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<USplineComponent*> RoadSplines;

	/** 도로 접속 감지용 콜리전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<UOmniDetectSphereComponent*> RoadConnectDetectors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<USplineComponent*> LaneSplines;

	/** 적용할 스태틱 메시. 휘어져야하기 때문에 버텍스가 충분해야함. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PlacedMesh;

	UPROPERTY()
	TMap<uint64, AOmniStationBusStop*> OwnedBusStops;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector BoxCollisionExtent;

	double BoxCollisionOffset;

	/** 도로 중앙선 개수 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint8 RoadSplineNum;
	/** 도로 연결 지점 감지 컴포넌트 개수*/
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint8 RoadConnectDetectorNum;
	/** 차선 개수 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint8 LaneSplineNum;
};
