// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniAsync.h"

#include <queue>

#include "OmnibusGameInstance.h"
#include "OmniLineBusRoute.h"
#include "OmniPassenger.h"
#include "OmniPlayMainUI.h"
#include "OmniStationBusStop.h"


UOmniPathFinder::UOmniPathFinder()
{
	TaskCount = FMath::Max(FPlatformMisc::NumberOfWorkerThreadsToSpawn() / 2, 2);
}

void UOmniPathFinder::Initialize()
{
	PrevActorTickHandle = FWorldDelegates::OnWorldPreActorTick.AddUObject(this, &UOmniPathFinder::OnWorldPreActorTick);
	PostActorTickHandle = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UOmniPathFinder::OnWorldPostActorTick);

	LevelInitializeHandle   = FOmniWorldDelegates::OnLevelInitialize.AddUObject(this, &UOmniPathFinder::LevelInitialize);
	LevelUninitializeHandle = FOmniWorldDelegates::OnLevelUninitialize.AddUObject(this, &UOmniPathFinder::LevelUninitialize);
	
	// 부하를 줄이기 위해 작업스레드 수의 절반만 사용
	TaskCount = FMath::Max(FPlatformMisc::NumberOfWorkerThreadsToSpawn() / 2, 2);
}

void UOmniPathFinder::BeginDestroy()
{
	FWorldDelegates::OnWorldPreActorTick.Remove(PrevActorTickHandle);
	FWorldDelegates::OnWorldPostActorTick.Remove(PostActorTickHandle);

	FOmniWorldDelegates::OnLevelInitialize.Remove(LevelInitializeHandle);
	FOmniWorldDelegates::OnLevelUninitialize.Remove(LevelUninitializeHandle);

	UObject::BeginDestroy();
}

void UOmniPathFinder::LevelInitialize(UOmnibusGameInstance* InOmniGameInstance)
{
}

void UOmniPathFinder::LevelUninitialize(UOmnibusGameInstance* InOmniGameInstance)
{
	// 레벨이 끝날 때, 비동기 데이터 정리.
	// todo: 레벨 끝날 때, 각 승객이 취소요청을 하지만, 전체 Abort 처리 혹은 레벨별 태그를 이용해서 자동 중단 처리가 필요할 수 있습니다.
	FOmniAsync::ClearDataMapAsync();
}

void UOmniPathFinder::OnWorldPreActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds)
{
	PerformPathFindingAsync();
}

void UOmniPathFinder::OnWorldPostActorTick(UWorld* World, ELevelTick TickType, float DeltaTime)
{
	FOmniAsync::DeliverDataMapAsync();
}

TQueryID UOmniPathFinder::FindTransferPathAsync(const FOmniPathFindingQuery& InPathQuery)
{
	PathFindingQueryList.Emplace(InPathQuery);

	UOmniPlayMainUI::OnPostRequest.Broadcast(1);
	UOmniPlayMainUI::OnPostFindPath.Broadcast(PathFindingQueryList.Last().StartList.Num() * PathFindingQueryList.Last().EndList.Num() * PathFindingQueryList.Last().TransferRuleList.Num());

	return PathFindingQueryList.Last().QueryID;
}

void UOmniPathFinder::RetryPathFinding(FOmniPathFindingQuery&& InPathQuery)
{
	RetryPathFindingQueryList.Emplace(MoveTemp(InPathQuery));

	UOmniPlayMainUI::OnPostRetry.Broadcast(RetryPathFindingQueryList.Last().TransferRuleList.Num());
}

void UOmniPathFinder::AbortPathFinding(const TQueryID InAbortID)
{
	bool bIsRetryQueryRemove = OmniContainer::RemoveByPredicate(RetryPathFindingQueryList, [InAbortID](const FOmniPathFindingQuery& InFindingQuery)
	{
		return InFindingQuery.QueryID == InAbortID;
	});
	
	bool bIsPathQueryRemove = OmniContainer::RemoveByPredicate(PathFindingQueryList, [InAbortID](const FOmniPathFindingQuery& InFindingQuery)
	{
		return InFindingQuery.QueryID == InAbortID;
	});

	if (bIsRetryQueryRemove == false && bIsPathQueryRemove == false)
		AbortPathFindingList.Emplace(InAbortID);
}

bool UOmniPathFinder::IsAbortPathFindingQuery(const TQueryID InAbortID) const
{
	return AbortPathFindingList.Contains(InAbortID);
}

void UOmniPathFinder::CompleteAbortRequest(const TQueryID InAbortID)
{
	AbortPathFindingList.Remove(InAbortID);
}

void UOmniPathFinder::PerformPathFindingAsync()
{
	if (RetryPathFindingQueryList.IsEmpty() && PathFindingQueryList.IsEmpty())
		return;

	static FAutoConsoleTaskPriority CPrio_PerformPathFindingAsync(
		TEXT("TaskGraph.TaskPriorities.PerformPathFindingAsync"),
		TEXT("Task and thread priority for UOmniPathFinder::PerformPathFindingAsync."),
		ENamedThreads::BackgroundThreadPriority,   // if we have background priority task threads, then use them...
		ENamedThreads::NormalTaskPriority,         // .. at normal task priority
		ENamedThreads::NormalTaskPriority          // if we don't have background threads, then use normal priority threads at normal task priority instead
	);

	// 재시도 쿼리부터 시행
	RetryPathFindingQueryList.Append(MoveTemp(PathFindingQueryList));

	// 비동기 작업 분산.
	// 먼저 들어온 쿼리부터 수행하기 위해, 순번을 건너뛰면서 나눔.  
	TArray<TArray<FOmniPathFindingQuery>> SplitQueues;
	SplitQueues.SetNum(TaskCount);
	for (TArray<FOmniPathFindingQuery>& SplitArray : SplitQueues)
	{
		SplitArray.Reset();
	}
	for (int32 i = 0; i < RetryPathFindingQueryList.Num(); ++i)
	{
		const int32 SplitIdx = i % TaskCount;
		SplitQueues[SplitIdx].Emplace(MoveTemp(RetryPathFindingQueryList[i]));
	}

	// TaskCount 만큼 분산되어 처리.
	TWeakObjectPtr<UOmniPathFinder> PathFinderWeak = this;
	for (TArray<FOmniPathFindingQuery>& SplitArray : SplitQueues)
	{
		AsyncTask(CPrio_PerformPathFindingAsync.Get(), [RetryAndNormalPathQueryList = MoveTemp(SplitArray), PathFinderWeak]() mutable
		{
			for (FOmniPathFindingQuery& PathQuery : RetryAndNormalPathQueryList)
			{
				UOmniPathFinder::ExecutePathFindingQuery(PathFinderWeak, PathQuery);
			}
		});
	}

	RetryPathFindingQueryList.Reset();
	PathFindingQueryList.Reset();
}

void UOmniPathFinder::ExecutePathFindingQuery(TWeakObjectPtr<UOmniPathFinder> InOmniPathFinderWeak, FOmniPathFindingQuery& InPathQuery)
{
	ensure(IsInGameThread() == false);

	const TArray<TWeakObjectPtr<AOmniStationBusStop>>& StartList = InPathQuery.StartList;
	const TArray<TWeakObjectPtr<AOmniStationBusStop>>& EndList   = InPathQuery.EndList;
	TArray<FTransferPath> ResultTransferPaths;
	EFindPath FindResult = EFindPath::None;

	int SuccessCount = 0;
	int NoPathCount  = 0;

	for (const TWeakObjectPtr<AOmniStationBusStop>& StartStopWeak : StartList)
	{
		if (FOmniStatics::IsNullWeak(StartStopWeak))
			continue;

		for (const TWeakObjectPtr<AOmniStationBusStop>& EndStopWeak : EndList)
		{
			if (FOmniStatics::IsNullWeak(EndStopWeak))
				continue;

			// 환승 규칙만큼 경로 탐색.
			FTransferPath TransferPath;
			for (FTransferRule& TransferRule : InPathQuery.TransferRuleList)
			{
				TransferPath.Reset();
				FindResult = FindPathByStop(StartStopWeak, EndStopWeak, TransferRule, TransferPath);

				// 중간에 업데이트가 있었다면 다시 시도.
				if (FindResult == EFindPath::Interruption_By_Update)
					break;

				if (TransferPath.IsValidPath())
					ResultTransferPaths.Emplace(MoveTemp(TransferPath));

				if (FindResult == EFindPath::Success)
					++SuccessCount;
				else if (FindResult == EFindPath::Fail_No_Path)
					++NoPathCount;
			}
		}

		if (FindResult == EFindPath::Interruption_By_Update)
			break;
	}

	if (FindResult == EFindPath::Interruption_By_Update)
	{
		ResultTransferPaths.Empty();
	}
	else
	{
		if (ResultTransferPaths.IsEmpty() == false)
		{
			Algo::Sort(ResultTransferPaths, [](const FTransferPath& InA, const FTransferPath& InB)
			{
				return InA.GetTotalPathDistance() < InB.GetTotalPathDistance();
			});
		}
	}

	// 게임쓰레드에서 실행
	AsyncTask(ENamedThreads::GameThread, [InOmniPathFinderWeak, FindResult, MovePathQuery = MoveTemp(InPathQuery), MoveTransferPaths = MoveTemp(ResultTransferPaths), SuccessCount, NoPathCount]() mutable
	{
		UOmniPathFinder* OmniPathFinder = InOmniPathFinderWeak.Get();
		if (OmniPathFinder == nullptr)
			return;

		// 취소된 찾기 요청인지 확인
		if (OmniPathFinder->IsAbortPathFindingQuery(MovePathQuery.QueryID))
		{
			OmniPathFinder->CompleteAbortRequest(MovePathQuery.QueryID);
			return;
		}

		// 데이터맵 업데이트로 인한 재시도.
		if (FindResult == EFindPath::Interruption_By_Update)
		{
			OmniPathFinder->RetryPathFinding(MoveTemp(MovePathQuery));
		}
		else
		{
			UOmniPlayMainUI::OnPostSuccessPath.Broadcast(SuccessCount);
			UOmniPlayMainUI::OnPostNoPath.Broadcast(NoPathCount);

			MovePathQuery.OnGameThreadDelegate.ExecuteIfBound(MovePathQuery.QueryID, MoveTemp(MoveTransferPaths));
		}
	});
}

EFindPath UOmniPathFinder::FindPathByStop(const TWeakObjectPtr<AOmniStationBusStop>& InStartBusStop
                                        , const TWeakObjectPtr<AOmniStationBusStop>& InEndBusStop
                                        , const FTransferRule& InTransferRules
                                        , FTransferPath& OutTransferPath)
{
	// 데이터 변경용 기준. 중간에 달라지면 다시 시작.
	const uint64 TicketCount = FOmniAsync::GetTicketCount();

	//~=============================================================================
	// Lambda

	// 목적지 위치 미리 캡쳐.
	// 휴리스틱. 목적지 까지의 예상 거리 계산. 유클리드 거리.
	FVector2D EndStopPos;
	{
		FPackReadLockGuard ReadLock(FOmniAsync::StopLockData);
		if (TicketCount != FOmniAsync::GetTicketCount())
			return EFindPath::Interruption_By_Update;

		const weak_map<AOmniStationBusStop, FStopData>& StopDataMap = FOmniAsync::StopLockData.DataMap;

		const auto FindIt = StopDataMap.find(InEndBusStop);
		if (FindIt == StopDataMap.end())
			return EFindPath::Fail_Invalid_Stop;

		EndStopPos = FindIt->second.StopLocation;
	}

	auto HeuristicToEndStop = [EndStopPos](const TWeakObjectPtr<AOmniStationBusStop>& InNextBusStop) -> double
	{
		FPackReadLockGuard ReadLock(FOmniAsync::StopLockData);
		const weak_map<AOmniStationBusStop, FStopData>& StopDataMap = FOmniAsync::StopLockData.DataMap;

		const auto FindIt = StopDataMap.find(InNextBusStop);
		if (FindIt == StopDataMap.end())
			return FOmniConst::GetBigNumber<double>();

		const FVector2D InStopStartPos = FindIt->second.StopLocation;

		return (EndStopPos - InStopStartPos).Size();
	};


	// 최단 경로 재구성 함수
	auto ReconstructPath = [](const weak_map<AOmniStationBusStop, FTransferStep>& InCameFromHow
	                        , TWeakObjectPtr<AOmniStationBusStop> InCurrent
	                        , FTransferPath& OutPath)
	{
		TArray<FTransferStep>& OutPathList = OutPath.TransferStepList;
		OutPathList.Reset();

		FTransferStep CurrentStep = FTransferStep::MakeLastStep(InCurrent);
		while (InCameFromHow.contains(CurrentStep.BusStop))
		{
			OutPathList.Emplace(CurrentStep);
			TWeakObjectPtr<AOmniStationBusStop> CurrentBusStop = CurrentStep.BusStop;
			
			OB_IF ( FOmniStatics::IsNullWeak(CurrentBusStop) )
				return;

			CurrentStep = InCameFromHow.at(CurrentBusStop);

			if (OutPathList.Num() > 1'000'000)
			{
				OB_LOG("Loop Max Over")
				OutPathList.Empty();
				return;
			}
		}
		OutPathList.Emplace(CurrentStep); // 시작 추가

		Algo::Reverse(OutPathList);
	};

	struct pair_greater
	{
		constexpr bool operator()(const std::pair<double, TWeakObjectPtr<AOmniStationBusStop>>& InLeft, const std::pair<double, TWeakObjectPtr<AOmniStationBusStop>>& InRight) const
		{
			return InLeft.first > InRight.first;
		}
	};

	// F = G + H
	// G = 현재 지점까지 실제 거리
	// H = 현재 지점에서 남은 예상 거리
	std::priority_queue<std::pair<double, TWeakObjectPtr<AOmniStationBusStop>>, std::vector<std::pair<double, TWeakObjectPtr<AOmniStationBusStop>>>, pair_greater> PriorityQ;
	weak_map<AOmniStationBusStop, FTransferStep> CameFromHow; // 경로로 가는 방법 추적. 'CameFromHow[B] = BusA_'는 B에 도달하려면, FTransferStep의 버스정류장에서 해당 버스노선을 이용해야한다는 의미
	weak_map<AOmniStationBusStop, double> F_DistStartToEnd;   // G(실제거리) + 휴리스틱 예상 남은 거리
	weak_map<AOmniStationBusStop, double> G_DistFromStart;    // 시작 노드로부터 현재까지 실제 거리

	PriorityQ.emplace(0.0, InStartBusStop);
	F_DistStartToEnd[InStartBusStop] = HeuristicToEndStop(InStartBusStop);
	G_DistFromStart[InStartBusStop]  = 0.0;

	while (PriorityQ.empty() == false)
	{
		const TWeakObjectPtr<AOmniStationBusStop> CurrentBusStop = PriorityQ.top().second;
		PriorityQ.pop();

		if (FOmniStatics::IsSameWeak(CurrentBusStop, InEndBusStop))
		{
			ReconstructPath(CameFromHow, CurrentBusStop, OutTransferPath);
			return EFindPath::Success;
		}

		TArray<TWeakObjectPtr<AOmniLineBusRoute>> BusRoutes;

		// 정류장 Lock Guard
		{
			FPackReadLockGuard ReadLock(FOmniAsync::StopLockData);

			if (TicketCount != FOmniAsync::GetTicketCount())
				return EFindPath::Interruption_By_Update;

			const weak_map<AOmniStationBusStop, FStopData>& StopDataMap = FOmniAsync::StopLockData.DataMap;

			const auto FindIt = StopDataMap.find(CurrentBusStop);
			OB_IF(FindIt == StopDataMap.end())
				continue;

			BusRoutes = FindIt->second.BusRoutes;
		}

		TArray<FBusStopDistance> BusStopDistanceList;
		float LineSplineLength = 0.0f;		
		for (TWeakObjectPtr<AOmniLineBusRoute>& BusLineWeak : BusRoutes)
		{
			BusStopDistanceList.Reset();
			LineSplineLength = 0.0f;

			// 노선 Lock Guard
			{
				FPackReadLockGuard ReadLock(FOmniAsync::LineLockData);

				if (TicketCount != FOmniAsync::GetTicketCount())
					return EFindPath::Interruption_By_Update;

				const weak_map<AOmniLineBusRoute, FLineData>& LineDataMap = FOmniAsync::LineLockData.DataMap;

				const auto FindIt = LineDataMap.find(BusLineWeak);
				OB_IF(FindIt == LineDataMap.end())
				{
					continue;
				}

				const FLineData& LineData = FindIt->second;

				BusStopDistanceList = LineData.BusStopDistanceList;
				LineSplineLength    = LineData.LineSplineLength;
			}

			for (int NextDistIdx = 0; NextDistIdx < BusStopDistanceList.Num(); ++NextDistIdx)
			{
				const FBusStopDistance& NextBusStopDist         = BusStopDistanceList[NextDistIdx];
				TWeakObjectPtr<AOmniStationBusStop> NextBusStop = NextBusStopDist.BusStop;
				if (FOmniStatics::IsNullWeak(NextBusStop) || FOmniStatics::IsSameWeak(CurrentBusStop, NextBusStop))
					continue;

				// 다음 정류장까지 가는 경로 및 거리
				auto [StartBusStopListIdx, DistToNextStop] = AOmniLineBusRoute::GetShortestStartIdxAndDist(BusStopDistanceList, LineSplineLength, CurrentBusStop, NextDistIdx);
				
				if (StartBusStopListIdx == INDEX_NONE)
					continue;

				// 내릴 땐 환승저항 추가 안함.
				float TransferResist = InTransferRules.GetResistance();
				if (FOmniStatics::IsSameWeak(NextBusStop, InEndBusStop))
					TransferResist = 0.0f;

				const double tentativeNextGDist = G_DistFromStart[CurrentBusStop] + DistToNextStop + TransferResist; // 다음 버스정류장 가는 거리. 환승 저항 추가.

				if (G_DistFromStart.contains(NextBusStop) == false || tentativeNextGDist < G_DistFromStart[NextBusStop])
				{
					CameFromHow[NextBusStop]      = FTransferStep(CurrentBusStop, BusLineWeak, StartBusStopListIdx, NextDistIdx, DistToNextStop);
					G_DistFromStart[NextBusStop]  = tentativeNextGDist;
					F_DistStartToEnd[NextBusStop] = G_DistFromStart[NextBusStop] + HeuristicToEndStop(NextBusStop);

					PriorityQ.emplace(F_DistStartToEnd[NextBusStop], NextBusStop);

					if (TicketCount != FOmniAsync::GetTicketCount())
						return EFindPath::Interruption_By_Update;
				}
			}
		}
	}

	return EFindPath::Fail_No_Path;
}

FLockPackage<AOmniStationBusStop, FStopData> FOmniAsync::StopLockData{};
FLockPackage<AOmniLineBusRoute, FLineData> FOmniAsync::LineLockData{};

std::atomic<uint64> FOmniAsync::TicketCount{0};

uint64 FOmniAsync::GetTicketCount()
{
	return TicketCount;
}

void FOmniAsync::AddTicketCount()
{
	TicketCount.fetch_add(1);
}

TSet<TWeakObjectPtr<AOmniStationBusStop>> FOmniAsync::StopUpdateQueue{};
TSet<TWeakObjectPtr<AOmniLineBusRoute>> FOmniAsync::LineUpdateQueue{};

TSet<TWeakObjectPtr<AOmniStationBusStop>> FOmniAsync::StopDeleteQueue{};
TSet<TWeakObjectPtr<AOmniLineBusRoute>> FOmniAsync::LineDeleteQueue{};

FOmniAsync::FOnPostDataUpdate FOmniAsync::OnPostDataUpdate;


template <class InKeyType, class InValueType>
using TWeakPairArray = TArray<TPair<TWeakObjectPtr<InKeyType>, InValueType>>;

void FOmniAsync::DeliverDataMapAsync()
{
	if (StopUpdateQueue.IsEmpty() && LineUpdateQueue.IsEmpty() && StopDeleteQueue.IsEmpty() && LineDeleteQueue.IsEmpty())
		return;

	// 복사를 줄이기 위해 TUniquePtr 사용.
	TUniquePtr<TWeakPairArray<AOmniStationBusStop, FStopData>> StopDataUniquePtr = MakeUnique<TWeakPairArray<AOmniStationBusStop, FStopData>>();
	TUniquePtr<TWeakPairArray<AOmniLineBusRoute, FLineData>> LineDataUniquePtr   = MakeUnique<TWeakPairArray<AOmniLineBusRoute, FLineData>>();

	// 업데이트 목록으로 데이터 생성
	for (TWeakObjectPtr<AOmniStationBusStop>& UpdateStopWeak : StopUpdateQueue)
	{
		if (const AOmniStationBusStop* UpdateStop = UpdateStopWeak.Get())
			StopDataUniquePtr.Get()->Emplace(UpdateStopWeak, FStopData{UpdateStop->GetBusRouteList(), FVector2D(UpdateStop->GetActorLocation())});
	}
	for (TWeakObjectPtr<AOmniLineBusRoute>& UpdateLineWeak : LineUpdateQueue)
	{
		if (const AOmniLineBusRoute* UpdateLine = UpdateLineWeak.Get())
			LineDataUniquePtr.Get()->Emplace(UpdateLineWeak, FLineData{UpdateLine->GetBusStopDistanceList(), UpdateLine->GetRouteLength()});
	}

	// Wait하지 않기 위해, 다른 스레드에서 데이터 업데이트. 즉시 업데이트를 위해 비교적 높은 우선순위 쓰레드 사용.
	// 삭제 데이터는 목록만 필요하기 때문에 이동처리.
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [InStopDataUniquePtr = MoveTemp(StopDataUniquePtr), InLineDataUniquePtr = MoveTemp(LineDataUniquePtr), DeleteLineList = MoveTemp(LineDeleteQueue), DeleteStopList = MoveTemp(StopDeleteQueue)]()
	{
		{
			FPackWriteLockGuard WriteLock(FOmniAsync::StopLockData);
			FOmniAsync::AddTicketCount();
			TWeakPairArray<AOmniStationBusStop, FStopData>& StopDataList = *InStopDataUniquePtr.Get();
			for (TPair<TWeakObjectPtr<AOmniStationBusStop>, FStopData>& StopDataPair : StopDataList)
			{
				FOmniAsync::StopLockData.DataMap[StopDataPair.Key] = MoveTemp(StopDataPair.Value);
			}
			for (const TWeakObjectPtr<AOmniStationBusStop>& DeleteStopWeak : DeleteStopList)
			{
				FOmniAsync::StopLockData.DataMap.erase(DeleteStopWeak);
			}
		}

		{
			FPackWriteLockGuard WriteLock(FOmniAsync::LineLockData);
			FOmniAsync::AddTicketCount();
			TWeakPairArray<AOmniLineBusRoute, FLineData>& LineDataList = *InLineDataUniquePtr.Get();
			for (TPair<TWeakObjectPtr<AOmniLineBusRoute>, FLineData>& LineDataPair : LineDataList)
			{
				FOmniAsync::LineLockData.DataMap[LineDataPair.Key] = MoveTemp(LineDataPair.Value);
			}
			for (const TWeakObjectPtr<AOmniLineBusRoute>& DeleteLineWeak : DeleteLineList)
			{
				FOmniAsync::LineLockData.DataMap.erase(DeleteLineWeak);
			}
		}

		AsyncTask(ENamedThreads::GameThread, []() mutable
		{
			// 데이터 업데이트 끝난 후 호출할 함수들 브로드캐스트
			FOmniAsync::OnPostDataUpdate.Broadcast();
		});
	});


	// 사용한 대기열은 모두 리셋합니다.
	StopUpdateQueue.Reset();
	LineUpdateQueue.Reset();
	StopDeleteQueue.Reset();
	LineDeleteQueue.Reset();
}

void FOmniAsync::ClearDataMapAsync()
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, []() mutable
	{
		{
			FPackWriteLockGuard WriteLock(FOmniAsync::StopLockData);
			FOmniAsync::AddTicketCount();
			weak_map<AOmniStationBusStop, FStopData> EmptyStopMap{};
			std::swap(FOmniAsync::StopLockData.DataMap, EmptyStopMap);
		}
		{
			FPackWriteLockGuard WriteLock(FOmniAsync::LineLockData);
			FOmniAsync::AddTicketCount();
			weak_map<AOmniLineBusRoute, FLineData> EmptyLineMap{};
			std::swap(FOmniAsync::LineLockData.DataMap, EmptyLineMap);
			// FOmniAsync::LineLockData.DataMap.clear();    // 버킷 사이즈 유지
		}
	});
}

void FOmniAsync::UpdateStopDataAsync(AOmniStationBusStop* InTargetStop)
{
	if (IsValid(InTargetStop) == false)
		return;

	// 삭제 리스트에 있다면 제거.
	StopDeleteQueue.Remove(InTargetStop);

	StopUpdateQueue.Emplace(InTargetStop);
}

void FOmniAsync::UpdateLineDataAsync(AOmniLineBusRoute* InTargetLine)
{
	if (IsValid(InTargetLine) == false)
		return;

	// 삭제 리스트에 있다면 제거.
	LineDeleteQueue.Remove(InTargetLine);

	LineUpdateQueue.Emplace(InTargetLine);
}

void FOmniAsync::DeleteStopDataAsync(AOmniStationBusStop* InTargetStop)
{
	if (IsValid(InTargetStop) == false)
		return;

	// 업데이트 리스트에 있다면 제거.
	StopUpdateQueue.Remove(InTargetStop);

	StopDeleteQueue.Emplace(InTargetStop);
}

void FOmniAsync::DeleteLineDataAsync(AOmniLineBusRoute* InTargetLine)
{
	if (IsValid(InTargetLine) == false)
		return;

	// 업데이트 리스트에 있다면 제거.
	LineUpdateQueue.Remove(InTargetLine);

	LineDeleteQueue.Emplace(InTargetLine);
}
