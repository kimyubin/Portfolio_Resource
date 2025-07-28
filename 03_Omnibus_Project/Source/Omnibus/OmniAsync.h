// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniAsyncTypes.h"
#include "OmniMacros.h"
#include "OmniObject.h"
#include "UObject/NoExportTypes.h"
#include "OmniAsync.generated.h"


/**
 * 비동기 길찾기 관리자 클래스입니다.
 * Non-GameThread 데이터맵 갱신, 길찾기 요청 쿼리 수집, 실행, 결과 반환 등 관련된 기능을 수행합니다. 
 */
UCLASS()
class OMNIBUS_API UOmniPathFinder : public UOmniObject
{
	GENERATED_BODY()

public:
	UOmniPathFinder();

	virtual void Initialize(UOmnibusGameInstance* InOmniGameInstance) override;
	virtual void BeginDestroy() override;

	void PostLevelBeginPlay();
	void LevelEnd();

	/** FWorldDelegates::OnWorldPreActorTick 이벤트 핸들러. 월드의 모든 액터보다 먼저 실행됩니다. */
	void OnWorldPreActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds);

	/** FWorldDelegates::OnWorldPostActorTick 이벤트 핸들러. 월드 모든 액터 틱이 완료된 다음 실행됩니다. */
	void OnWorldPostActorTick(UWorld* World, ELevelTick TickType, float DeltaTime);

	/**
	 *  길찾기 요청 쿼리를 큐에 등록합니다. 쿼리는 다음 틱에서 실행됩니다.
	 *  
	 * @param InPathQuery 출발, 도착지, 환승룰과 쿼리가 완료된 다음 호출될 델리게이트를 포함합니다.
	 * @return 쿼리 ID를 반환합니다. ID를 통해 쿼리를 취소할 수 있습니다.
	 */
	TQueryID FindTransferPathAsync(const FOmniPathFindingQuery& InPathQuery);

	/** 데이터맵 변경 등의 이유로 중단된 쿼리를 재시도 큐에 넣어 관리합니다. 다음 틱에서 다시 시도합니다. */
	void RetryPathFinding(FOmniPathFindingQuery&& InPathQuery);

	/**
	 * 길찾기 쿼리 취소 요청
	 * 아직 수행되지 않았다면 리스트에서 제거하고,
	 * 이미 시작했다면, 쿼리 바운딩한 델리게이트의 실행을 막습니다.
	 */
	void AbortPathFinding(const TQueryID InAbortID);

	/** 쿼리가 취소 대상 목록에 있는지 확인합니다. */
	bool IsAbortPathFindingQuery(const TQueryID InAbortID) const;

	/**
	 * 델리게이트 실행을 정상적으로 중단한 경우 호출합니다.
	 * 취소 대상 목록에서 쿼리를 삭제합니다.
	 * 취소를 반려하는 기능으로 사용될 수 없습니다.
	 */
	void CompleteAbortRequest(const TQueryID InAbortID);

	/** 길찾기 쿼리를 순차적으로 실행합니다. */
	void PerformPathFindingAsync();

	/**
	 * 주어진 쿼리로 길찾기를 수행하고, 델리게이트를 실행합니다.
	 * Non-GameThread에서 사용됩니다.
	 */
	static void ExecutePathFindingQuery(TWeakObjectPtr<UOmniPathFinder> InOmniPathFinderWeak
	                                  , FOmniPathFindingQuery& InPathQuery);


	/**
	 * 정류장에서 정류장으로 갈 수 있는 환승 경로를 찾습니다.
	 */
	static EFindPath FindPathByStop(const TWeakObjectPtr<AOmniStationBusStop>& InStartBusStop
	                              , const TWeakObjectPtr<AOmniStationBusStop>& InEndBusStop
	                              , const FTransferRule& InTransferRules
	                              , FTransferPath& OutTransferPath);

protected:
	/** 다음 업데이트에서 처리하기 위해 대기 중인 비동기 길찾기 쿼리입니다. */
	TArray<FOmniPathFindingQuery> PathFindingQueryList;

	/**
	 * 변경 사항 등으로 실패한 쿼리는 이곳에 등록됩니다.
	 * 일반 쿼리보다 먼저 실행됩니다.
	 */
	TArray<FOmniPathFindingQuery> RetryPathFindingQueryList;

	/**
	 * 취소 요청된 쿼리 중, 쿼리 리스트에서 제거되지 못한 요청은 이곳에 등록됩니다.
	 * 해당 쿼리가 완료된 다음, 호출되는 델리게이트의 실행을 중단합니다.
	 */
	TSet<TQueryID> AbortPathFindingList;

	/** 분산 작업 개수 */
	int32 TaskCount;


	FDelegateHandle PrevActorTickHandle;
	FDelegateHandle PostActorTickHandle;
	FDelegateHandle PostLevelBeginPlayHandle;
	FDelegateHandle LevelEndHandle;
};



/**
 * 데이터 동기화 작업을 관리하는 구조체입니다.
 * Non-GameThread에서 사용할 노선 정보의 복사본과 뮤텍스를 관리합니다.
 * 게임쓰레드의 Wait을 방지하기 위해, 등록절차는 다른 쓰레드에서 이루어집니다.
 */
struct FOmniAsync
{
	//~=============================================================================
	// Non-GameThread Proxy Data
	/**
	 * Non-GameThread에서 사용하는 정류장/노선 데이터 map과 그 뮤텍스입니다.
	 * key: weakptr, value: 정류장/노선 데이터 복사본
	 */
	static TLockPackage<AOmniStationBusStop, FStopProxy> StopProxyLockPack;
	static TLockPackage<AOmniLineBusRoute, FLineProxy> LineProxyLockPack;

	/**
	 * Non-GameThread Proxy 데이터 변경 확인용
	 * 데이터가 변경되면 값이 변경됩니다.
	 * ReadLock에서 wait이 종료될 때 확인해야합니다.
	 */
	static std::atomic<uint64> TicketCount;
	static uint64 GetTicketCount();
	static void AddTicketCount();

	//~=============================================================================
	// DeferredQueue
	/**
	 * 업데이트/삭제가 발생한 정류장, 노선 큐입니다.
	 * 모아서 이번 Tick 마지막에 한번에 업데이트 합니다.
	 * GameThread에서 사용됩니다.
	 */
	static TSet<TWeakObjectPtr<AOmniStationBusStop>> StopUpdateQueue;
	static TSet<TWeakObjectPtr<AOmniLineBusRoute>> LineUpdateQueue;

	static TSet<TWeakObjectPtr<AOmniStationBusStop>> StopDeleteQueue;
	static TSet<TWeakObjectPtr<AOmniLineBusRoute>> LineDeleteQueue;


	/** DataMap이 업데이트된 이후 호출되는 델리게이트 */
	DECLARE_MULTICAST_DELEGATE(FOnPostDataUpdate);
	static FOnPostDataUpdate OnPostDataUpdate;


	//~=============================================================================
	// 함수

	/** 비동기 길찾기에 사용되는 갱신된 데이터 복사본을 Non-GameThreads에 전달합니다. */
	static void DeliverProxyDataAsync();

	/** 비동기 길찾기 데이터를 지웁니다. */
	static void ClearProxyDataAsync();
	
	/**
	 * 비동기 길찾기에 사용할 정류장/노선의 데이터를 비동기로 업데이트합니다.
	 * 틱 마지막에 업데이트 됩니다.
	 * 따라서, 변경이 있는 함수 내부라면 어느 위치에 호출되어도 결과는 같습니다.
	 * (마킹만 함) 지연된 데이터 갱신
	 *
	 * 업데이트 데이터와 삭제 데이터는 서로 다른 값을 갖습니다.
	 * 같은 틱에서 삭제와 동시에 업데이트하는 객체는 없습니다.
	 * 더 나중에 들어온 데이터만 갱신됩니다.
	 */
	static void UpdateStopProxyAsync(AOmniStationBusStop* InTargetStop);
	static void UpdateLineProxyAsync(AOmniLineBusRoute* InTargetLine);

	static void DeleteStopProxyAsync(AOmniStationBusStop* InTargetStop);
	static void DeleteLineProxyAsync(AOmniLineBusRoute* InTargetLine);

};
