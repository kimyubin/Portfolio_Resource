// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniAsyncTypes.h"
#include "OmnibusTypes.h"
#include "OmniPawn.h"
#include "OmniPassenger.generated.h"

struct FTransferRule;
enum class EFindPath : uint8;
class APathVisualizator;
class USphereComponent;
class AOmniVehicleBus;
class USplineComponent;
class AOmniStationBusStop;
struct FTransferPath;
struct FTransferStep;
class AOmniCityBlock;
/**
 * 
 */
UCLASS()
class OMNIBUS_API AOmniPassenger : public AOmniPawn
{
	GENERATED_BODY()

public:
	AOmniPassenger();

protected:
	virtual void BeginPlay() override;
	virtual void PostBeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void BeginCursorOver(UPrimitiveComponent* TouchedComponent);
	
	UFUNCTION(BlueprintCallable)
	void EndCursorOver(UPrimitiveComponent* TouchedComponent);

	void DisablePassenger();
	void EnablePassenger();

	void SetMeshVisibility(const bool InVisibility);
	void SetMaterialColor(const FLinearColor& InLinearColor);
	void SetJourneyState(const EJourneyState InJourneyState);
	
	/**
	 * 시작, 목적지 목록 초기화.
	 * 환승 경로 얻은 후, 여행 정보 초기화 및 여정 시작
	 */
	void InitStartJourney(const FSectorInfo& InHomeInfo, const TArray<FSectorInfo>& InDestInfoList);

	/**
	 * 현재 위치에서 여정 시작을 다시 시도합니다.
	 * 환승 경로를 재탐색합니다.
	 */
	void RetryJourney();
	void RetryJourneyFromStop();
	void RetryJourneyFromSector();
	
	
	/**
	 * StartSector와 TargetSector 간의 경로를 찾고, 여행을 시작합니다.
	 * StartSector/TargetSector는 현재 여행 정보를 기반으로 결정됩니다.(Home/DestSector)
	 */
	void FindPathAndStartJourney();

	/** 두 Sector 간의 환승 경로를 찾고, 여행을 시작합니다.*/
	void FindPathAndStartJourneyBetweenSectors(const FSectorInfo& InStartInfo, const FSectorInfo& InEndInfo);

	/** 현재 버스 정류장에서 목표 섹터까지 경로를 찾고, 여행을 시작합니다. */
	void FindPathAndStartJourneyStopToSector(AOmniStationBusStop* InStartStop, const FSectorInfo& InEndInfo);


	/**
	 * @InStartList에서 @InEndList까지의 환승 경로를 비동기로 찾습니다.
	 * N:M으로 경로를 모두 찾습니다.
	 * 
	 * @param InStartList 시작 정류장 배열
	 * @param InEndList 도착 정류장 배열
	 * @param InTransferRuleList 환승 저항 등을 저장하는 환승규칙 목록
	 * @param InGameThreadDelegate 쿼리가 완료되면 GameThread에서 호출되는 델리게이트입니다. 쿼리가 실패하더라도 호출됩니다.
	 *                             Abort 요청이 있다면 실행되지 않습니다.
	 */
	void FindPathAsync(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InStartList
	                 , const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InEndList
	                 , const TArray<FTransferRule>& InTransferRuleList
	                 , const FPathFindingQueryDelegate& InGameThreadDelegate);


	/**
	 * 섹터간 환승 경로 찾기를 완료한 후, 게임스레드에서 실행되는 함수입니다.
	 *
	 * @note FindPathAndStartJourneyBetweenSectors()에서 사용됩니다.
	 * @param InQueryID 요청한 쿼리의 ID
	 * @param InTransferPath 길찾기 결과 경로
	 */
	void OnGameThreadBetweenSectors(TQueryID InQueryID, TArray<FTransferPath>&& InTransferPath);

	/**
	 * 단일 정류장에서 섹터까지 환승 경로 찾기를 완료한 후, 게임스레드에서 실행되는 함수입니다.
	 *
	 * @note FindPathAndStartJourneyStopToSector()에서 사용됩니다.
	 * @param InQueryID 요청한 쿼리의 ID
	 * @param InTransferPath 길찾기 결과 경로
	 */
	void OnGameThreadStopToSector(TQueryID InQueryID, TArray<FTransferPath>&& InTransferPath);

	/**
	 * 환승 경로 결과를 업데이트합니다.
	 * 경로가 없다면 경로없음 목록에 등록합니다.
	 * 
	 * @param InQueryID 요청한 쿼리의 ID
	 * @param InTransferPath 길찾기 결과 경로. 없다면 경로없음 목록에 등록합니다.
	 * @return 쿼리 ID와 경로가 유효하다면 true입니다.
	 */
	bool UpdateTransferPath(TQueryID InQueryID, TArray<FTransferPath>&& InTransferPath);
	
	/**
	 * 블록에서 출발하기. 주어진 환승 경로를 통해 여정을 시작합니다.
	 * 환승 경로 길찾기가 완료된 후 호출됩니다.
	 */
	void GoFromSector();

	/**
	 * 정류장에서 출발하기. 주어진 환승 경로를 통해 여정을 시작합니다.
	 * 환승 경로 길찾기가 완료된 후 호출됩니다.
	 */
	void GoFromStop();

	/**
	 * 이번에 정차한 버스에 승차하는 경로를 반환합니다.
	 * 
	 * @param InThisBus 정차한 버스 
	 * @return 있다면 PathList의 Idx를 반환하고, 없다면 INDEX_NONE를 반환합니다. 
	 */
	int32 GetPathIdxByThisBus(const AOmniVehicleBus* InThisBus);

	/**
	 * 정류장이 호출. 이번에 정차한 버스에 승차하는지 확인합니다.
	 * 
	 * @param InThisBus 정차 중인 버스
	 * @return 이번에 승차한다면 true입니다.
	 */
	bool IsEntryToThisBus(const AOmniVehicleBus* InThisBus);

	/**
	 * 버스가 호출. 이번 정류장에서 하차한다면 하차 절차를 밟습니다.
	 * 
	 * @param InThisBusStop 정차한 정류장
	 * @return 이번에 하차한다면 true입니다.
	 */
	bool IsExitToThisBusStop(const AOmniStationBusStop* InThisBusStop);

	/**
	 * 승차 절차를 개시하고, 지정된 지연 시간 이후 버스에 승차합니다.
	 * 
	 * @param InThisBus 승차할 버스
	 * @param InEntryDelay 지정된 지연시간. 탑승하는 데 걸리는 시간입니다.
	 */
	void DoEntryToBus(AOmniVehicleBus* InThisBus, const float InEntryDelay);

	/**
	 * 하차 절차를 개시하고, 지정된 지연 시간 이후 정류장으로 하차합니다.
	 * @param InThisBusStop 하차할 정류장
	 * @param InExitDelay 지정된 지연시간. 하차하는 데 걸리는 시간입니다.
	 */
	void DoExitToStop(AOmniStationBusStop* InThisBusStop, const float InExitDelay);

private:
	void DoExitToStopImpl(AOmniStationBusStop* InThisBusStop);

public:
	/** 목적지 도착 절차 수행 */
	void ArriveDestination();


	/**
	 * TransferPathList(환승 목록)에서 환승 경로 안에 있는 환승 스탭을 가져옵니다.
	 * 
	 * @param InPathListIdx TransferPathList의 인덱스
	 * @param InStepListIdx TransferPathList-> TransferStepList의 인덱스
	 * @return 인덱스가 유효하지 않다면 nullptr을 반환합니다. 유효하다면 스탭의 포인터를 반환합니다.
	 */
	FTransferStep* GetTransferStep(const int32 InPathListIdx, const int32 InStepListIdx);

	/** 현재 환승 정보 포인터 */
	FTransferStep* GetCurrentTransferStep();

	/**
	 * 다음 정류장을 반환합니다.
	 * 노선이 변경되어도 고정된 목적지를 제공하기 위해, 다음 step의 승차 정류장에서 가져옵니다.
	 */
	AOmniStationBusStop* GetNextStop();
	
	/**
	 * z축 오프셋을 추가한 값으로 트랜스폼을 변경합니다.
	 * Scale은 무시합니다.
	 */
	void SetTransformAndOffsetNoScale(FTransform InTransform, const float InZ_Offset = 200.0f);

	/**
	 * PathQueryID를 갱신합니다.
	 * 기존에 요청한 쿼리가 있다면, 해당 쿼리를 취소합니다.
	 */
	void UpdatePathQueryID(const TQueryID InPathQueryID);

	/**
	 * 길찾기요청을 취소하고 PathQueryID를 초기화합니다.
	 * 대기열에 있다면 삭제하고, 이미 실행 중이라면 Delegate 실행을 무시합니다.
	 * 취소하는 시점에서 이미 이전 요청이 완료되었다면, false를 반환합니다
	 * @return 취소할 ID가 없다면, false입니다.
	 */
	bool AbortPathFinding();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* PassengerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputMeshComponent;

protected:
	/**
	 * 최초 스폰되는 곳이자, 복귀하는 곳입니다.
	 * 주거구역일 필요는 없습니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FSectorInfo HomeSector;

	/** 목적지 블록 목록 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FSectorInfo> DestSectorList;

	/** 개인 환승 성향*/
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FTransferRule> MyTransferRuleList;

	/** 요청한 길찾기 쿼리 ID */
	// UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TQueryID PathQueryID;

	/**
	 * 환승 경로 목록
	 * TArray<TArray<FTransferStep>>와 같습니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FTransferPath> TransferPathList;


	//~=============================================================================
	// 현재 상태
	EJourneyState CurrentJourneyState;
	
	/** 현재 어디에 있는 지를 나타냅니다. */
	FPassengerLocationInfo CurrentLocationInfo;
	
	/** 지금 가고 있는 환승 경로의 인덱스 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 PathListIdx;

	/**
	 * 지금 가고 있는 환승 절차의 인덱스
	 * TArray<TArray<FTransferStep>>[PathListIdx][StepListIdx]
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 StepListIdx;

	/** 여정 재시작시 사용하는 타이머 핸들 */
	FTimerHandle JourneyRepeatHandle;

	/** 현재 여정의 방향 상태 */
	EJourneyDirection CurrentJourneyDirection;

	/**
	 * 여정 방향을 스위칭합니다.
	 * HomeToDest <-> DestToHome
	 */
	void SwitchJourneyDirection();
	
	/** 현재 여행의 시작 요일 */
	EDay CurrentJourneyStartDay;

	/**
	 * 현재 여정 상태와 시작 요일을 바탕으로
	 * 여행 시작 지점 정보를 가져옵니다.
	 */
	FSectorInfo GetStartSectorInfo();
	FSectorInfo GetTargetSectorInfo();

	/**
	 * 마지막 경로 찾기에서 사용한 노선도의 버전입니다.
	 * 하차 후, 로드매니저의 노선도 버전과 비교해 차이가 있다면 길찾기를 다시 수행합니다.
	 */
	uint64 MyLastRouteMapVersion;

	/** 노선도가 변경되었습니다 */
	bool IsRouteMapUpdated() const;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<APathVisualizator> PathVisualizator;
};
