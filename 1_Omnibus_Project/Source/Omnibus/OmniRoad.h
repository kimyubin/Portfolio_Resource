// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmniRoad.generated.h"

struct FBusStopDistance;
class UOmniConnectorComponent;
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

	/** 이웃하는 도로 감지 및 추가. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = PostEditMove)
	virtual void DetectAllConnectedOmniRoad();

protected:
	/**
	 * 컴포넌트 개수를 결정하는 변수 초기화 및 다음 TArray를 해당 개수만큼 초기화.
	 * @param InRoadSplineNum          : 도로 스플라인 개수. RoadSplines
	 * @param InRoadConnectorNum : 도로 연결 감지 컴포넌트 개수. RoadConnectors, ConnectedRoadsArray
	 * @param InLaneSplineNum          : 차선 개수. LaneSplines
	 */
	void InitArrays(const uint8 InRoadSplineNum
	              , const uint8 InRoadConnectorNum
	              , const uint8 InLaneSplineNum);

	void InitRoadSpline(const int32 InIdx);
	void InitRoadConnector(const int32 InIdx, USplineComponent* InOwnerRoadSpline, const int32 InSplinePointIdx);
	void InitLaneSpline(const int32 InIdx, USplineComponent* InOwnerRoadSpline);

public:
	/**
	 * 이전 도로와 다음 도로를 이어주는 자신의 차선 반환.
	 * @param InPrevRoad 접근한 도로.
	 * @param InNextTargetRoad 다음 이동해야할 도로
	 * @return 다음 차선 lane
	 */
	USplineComponent* GetSplineToNextRoad(AOmniRoad* InPrevRoad, AOmniRoad* InNextTargetRoad);

	/**
	 * Lane 차선으로 갈 수 있는 다음도로 반환. LaneIdx 기반.
	 */
	virtual AOmniRoad* GetNextRoadByLaneIdx(const int32 InLaneIdx);
	
	/** 다음 차선 번호 찾기 */
	virtual int32 FindLaneIdxToNextRoad(AOmniRoad* InPrevRoad
	                                  , AOmniRoad* InNextTargetRoad);

	/**
	 * 차선 스플라인과 그 경로 위에 있는 버스 정류장을 순서대로 반환.
	 * 
	 * @param InLaneIdx 차선 인덱스 
	 * @return 0 : USplineComponent* - 차선 스플라인 
	 * @return 1 : TArray<FBusStopDistance> - 차선의 진행 방향으로 정렬된 버스 정류장 배열
	 */
	std::tuple<USplineComponent*, TArray<FBusStopDistance>> GetLaneAndBusStop(const int32 InLaneIdx) const;

	USplineComponent* GetRoadSpline(const int32 InIdx) const;
	USplineComponent* GetLaneSpline(const int32 InIdx) const;

	virtual void AddConnectedRoadSingle(AOmniRoad* InRoad, const int32 InAccessIdx);
	void AddConnectedRoadBoth(const UOmniConnectorComponent* InTargetConnector, const int32 InMyAccessIdx);

protected:
	void RemoveConnectedRoadSingle(const int32 InAccessIdx);
	void RemoveConnectedRoadSingle(AOmniRoad* InRoad);
	void RemoveConnectedRoadBoth(const int32 InAccessIdx);
	void RemoveConnectedRoadBoth(AOmniRoad* InRoad);
	/** 모든 연결 도로 양방향 제거. */
	void RemoveAllConnectedRoadsBoth();

public:
	/** ID로 버스정류장 찾아서 반환. 실패하면 nullptr 반환. */
	AOmniStationBusStop* FindBusStopByID(const uint64 InId);

	/**
	 * 정류장 Index 찾기
	 * @param InBusStop 찾을 버스 정류장
	 * @return 정류장의 Index. 찾지 못하면 INDEX_NONE.
	 */
	int32 FindBusStopIdx(AOmniStationBusStop* InBusStop) const;

	bool HasBusStopByID(const uint64 InId);
	bool HasBusStop(AOmniStationBusStop* InBusStop) const;

	void AddBusStop(AOmniStationBusStop* InBusStop);

	void RemoveBusStopByID(const uint64 InId);
	void RemoveBusStop(AOmniStationBusStop* InBusStop);
	void RemoveInvalidBusStop();

	TArray<TWeakObjectPtr<AOmniStationBusStop>> GetOwnedBusStops();
	
	/** 도로 포인터로 연결된 도로 Idx 가져오기. 실패하면 -1 반환. */
	int32 FindConnectedRoadIdx(AOmniRoad* InRoad) const;

	/**
	 * 다음으로 진행할 도로 반환
	 * @param InPrevRoad 직전 도로(진입한 도로)
	 * @return 다음 도로 (현재 차선을 타고 진출할 도로) 
	 */
	AOmniRoad* GetRandomNextRoad(AOmniRoad* InPrevRoad);

	AOmniRoad* GetConnectedRoad(const int32 ConnectedRoadIdx) const;

	int32 GetConnectedRoadNum() const { return ConnectedRoadsArray.Num(); };

	uint8 GetLaneSplineNum() const { return LaneSplineNum; };

	/** 현재 사용하는 메쉬를 기반으로 도로폭 계산 */
	virtual double GetRoadWidth();

protected:
	/**
	 * 오버랩된 감지 컴포넌트 탐지 및 도로 연결 상태 갱신
	 * @param Connector 탐지할 컴포넌트
	 * @return 탐지된 컴포넌트, 없다면 nullptr
	 */
	UOmniConnectorComponent* DetectOmniRoad(UOmniConnectorComponent* Connector);

public:
	/**
	 * 버스 정류장을 설치할 수 있는 지 확인합니다.
	 * 
	 * @return 정류장을 설치할 수 있다면 true.
	 */
	virtual bool CanInstallBusStop() const;;

	/**
	 * 액터 배열에서 가장 가까운 차선과 해당 차선을 보유한 도로를 찾음
	 * @param InActors      찾을 액터 배열
	 * @param InTargetPos   기준이 되는 위치. 이곳에서 가장 가까운 곳을 찾음
	 * @param OutNearRoad   가장 가까운 도로. OutLaneIdx 차선을 보유함.
	 * @param OutLaneIdx    가장 가까운 차선의 Index.
	 * @return 목록에서 가까운 도로 찾기 성공시 true.
	 */
	static bool FindNearestRoadAndLane(const TArray<AActor*>& InActors, const FVector& InTargetPos, AOmniRoad*& OutNearRoad, int32& OutLaneIdx);

protected:
	/** 연결되어있는 도로 집합 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<AOmniRoad*> ConnectedRoadsArray;

	/** 도로를 구성하는 기준 스플라인. 도로 중앙.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<USplineComponent*> RoadSplines;

	/** 도로 접속 감지용 콜리전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<UOmniConnectorComponent*> RoadConnectors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<USplineComponent*> LaneSplines;

	/** 적용할 스태틱 메시. 휘어져야하기 때문에 버텍스가 충분해야함. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PlacedMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniStationBusStop>> OwnedBusStops;

	/** 도로 중앙선 개수 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint8 RoadSplineNum;
	/** 도로 연결 지점 감지 컴포넌트 개수*/
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint8 RoadConnectorNum;
	/** 차선 개수 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint8 LaneSplineNum;
};
