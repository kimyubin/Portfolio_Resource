// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <unordered_map>

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "UObject/NoExportTypes.h"
#include "OmniAsyncTypes.generated.h"

class AOmniPassenger;
class AOmniStationBusStop;
class UOmnibusGameInstance;
class AOmniLineBusRoute;


/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmniAsyncTypes : public UObject
{
	GENERATED_BODY()
	
};


/** 약포인터 해시 제공 */
template<class T>
struct weak_hasher
{
	size_t operator()(const TWeakObjectPtr<T>& InWeak) const
	{
		// uint32(ObjectIndex ^ ObjectSerialNumber)
		return GetTypeHash(InWeak);
	}
};

/** 약포인터 동등성 체크. ThreadSafe를 위한 유효성 검사 없는 버전. */
template<class T>
struct weak_eq
{
	bool operator()(const TWeakObjectPtr<T>& AStop, const TWeakObjectPtr<T>& BStop) const
	{
		return FOmniStatics::IsSameWeak(AStop, BStop);
	}
};

/**
 * WeakPtr를 키로 사용하는 ThreadSafe 버전 unordered_map 입니다.
 * WeakPtr용 hasher와 ThreadSafe 동등성 평가 functor가 적용되었습니다.
 * 
 * @tparam InKeyType   key로 사용할 weakptr이 가리키는 자료형입니다. TWeakObjectPtr<InKeyType> 형태로 사용됩니다.
 * @tparam InValueType weak_map에 보관할 value 자료형입니다.
 */
template<typename InKeyType, typename InValueType>
using weak_map = std::unordered_map<TWeakObjectPtr<InKeyType>, InValueType, weak_hasher<InKeyType>, weak_eq<InKeyType>>;



/** 노선이 소유한 데이터 */
struct FLineData
{
	FLineData();

	explicit FLineData(const TArray<FBusStopDistance>& InBusStopDistanceList, const float InLineSplineLength);
	explicit FLineData(TArray<FBusStopDistance>&& InBusStopDistanceList, const float InLineSplineLength);

	FLineData(const FLineData& Other);
	FLineData(FLineData&& Other) noexcept;

	FLineData& operator=(const FLineData& Other);
	FLineData& operator=(FLineData&& Other) noexcept;

	TArray<FBusStopDistance> BusStopDistanceList;
	float LineSplineLength;
};

/** 정류장이 소유한 데이터 */
struct FStopData
{
	FStopData();

	explicit FStopData(const TArray<TWeakObjectPtr<AOmniLineBusRoute>>& InBusRoutes, const FVector2D& InStopLocation);
	explicit FStopData(TArray<TWeakObjectPtr<AOmniLineBusRoute>>&& InBusRoutes, FVector2D&& InStopLocation);

	FStopData(const FStopData& Other);
	FStopData(FStopData&& Other) noexcept;

	FStopData& operator=(const FStopData& Other);
	FStopData& operator=(FStopData&& Other) noexcept;

	TArray<TWeakObjectPtr<AOmniLineBusRoute>> BusRoutes;
	FVector2D StopLocation;
};


/**
 * 맵과 맵에 대한 뮤텍스를 묶은 구조체 입니다.
 * 
 * @tparam InKeyType   weak_map의 key타입입니다. weakptr이 참조하는 원본 데이터의 자료형입니다. 
 * @tparam InValueType weak_map에 보관할 데이터형입니다.
 */
template <typename InKeyType, typename InValueType>
struct FLockPackage
{
	explicit FLockPackage()
		: DataMap(weak_map<InKeyType, InValueType>()) {}

	/**
	 * key - 대상의 weakPtr입니다. ThreadSafe를 위해, 해싱과 비교에 Index와 시리얼만 사용합니다.
	 * value - 각 객체가 가지고 있는 데이터의 복사본입니다.
	 */
	weak_map<InKeyType, InValueType> DataMap;


	/**
	 * 맵과 원소에 대한 RW 뮤텍스입니다.
	 * 조회만 하는 작업에는 ReadLock을 사용하고,
	 * 그 외의 작업에는 Write를 사용해야합니다.
	 */
	mutable FRWLock MapRWLock;
};

/** 패키지 내부에 있는 뮤텍스의 읽기 전용 Lock Guard입니다. */
template <typename InKeyType, typename InValueType>
class FPackReadLockGuard
{
public:
	UE_NONCOPYABLE(FPackReadLockGuard);

	UE_NODISCARD_CTOR explicit FPackReadLockGuard(FLockPackage<InKeyType, InValueType>& InLockPack)
		: Lock(InLockPack.MapRWLock)
	{
		Lock.ReadLock();
	}

	~FPackReadLockGuard()
	{
		Lock.ReadUnlock();
	}

private:
	FRWLock& Lock;
};

/** 패키지 내부에 있는 뮤텍스의 쓰기 Lock Guard입니다. */
template <typename InKeyType, typename InValueType>
class FPackWriteLockGuard
{
public:
	UE_NONCOPYABLE(FPackWriteLockGuard);

	UE_NODISCARD_CTOR explicit FPackWriteLockGuard(FLockPackage<InKeyType, InValueType>& InLockPack)
		: Lock(InLockPack.MapRWLock)
	{
		Lock.WriteLock();
	}

	~FPackWriteLockGuard()
	{
		Lock.WriteUnlock();
	}

private:
	FRWLock& Lock;
};


using TQueryID = uint64;

enum { INVALID_QUERY_ID = 0 };

DECLARE_DELEGATE_TwoParams(FPathFindingQueryDelegate, TQueryID, TArray<FTransferPath>&&);

struct FOmniPathFindingQuery
{
	FOmniPathFindingQuery(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InStartList
	                    , const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InEndList
	                    , const TArray<FTransferRule>& InTransferRuleList
	                    , const FPathFindingQueryDelegate& InOnGameThreadDelegate);

	FOmniPathFindingQuery(const FOmniPathFindingQuery& Other);
	FOmniPathFindingQuery(FOmniPathFindingQuery&& Other) noexcept;

	FOmniPathFindingQuery& operator=(const FOmniPathFindingQuery& Other);
	FOmniPathFindingQuery& operator=(FOmniPathFindingQuery&& Other) noexcept;


	/** 취소할 때 사용할 수 있는 ID */
	TQueryID QueryID;

	/** 출발지와 도착지는 여러 곳일 수 있습니다. 모든 경우의 수를 찾습니다. */
	TArray<TWeakObjectPtr<AOmniStationBusStop>> StartList;
	TArray<TWeakObjectPtr<AOmniStationBusStop>> EndList;

	/** 환승 저항 등을 포함하는 환승 성향을 나타내는 규칙 목록입니다. 각 규칙에 맞는 환승 경로를 모두 찾습니다. */
	TArray<FTransferRule> TransferRuleList;

	/** 길찾기가 완료된 이후, 게임쓰레드에서 실행되는 델리게이트입니다. */
	FPathFindingQueryDelegate OnGameThreadDelegate;

protected:
	FORCEINLINE static TQueryID GetUniqueQueryID()
	{
		return ++LastPathQueryUniqueID;
	}

	static TQueryID LastPathQueryUniqueID;
};

