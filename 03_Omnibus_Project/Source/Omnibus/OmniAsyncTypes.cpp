// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniAsyncTypes.h"

#include "OmniPassenger.h"

TQueryID FOmniPathFindingQuery::LastPathQueryUniqueID = INVALID_QUERY_ID;


//~=============================================================================
// FLineProxy
FLineProxy::FLineProxy()
	: BusStopDistanceList()
	, LineSplineLength(0.0f) {}

FLineProxy::FLineProxy(const TArray<FBusStopDistance>& InBusStopDistanceList, const float InLineSplineLength)
	: BusStopDistanceList(InBusStopDistanceList)
	, LineSplineLength(InLineSplineLength) {}

FLineProxy::FLineProxy(TArray<FBusStopDistance>&& InBusStopDistanceList, const float InLineSplineLength)
	: BusStopDistanceList(MoveTemp(InBusStopDistanceList))
	, LineSplineLength(InLineSplineLength) {}

FLineProxy::FLineProxy(const FLineProxy& Other)
	: BusStopDistanceList(Other.BusStopDistanceList)
	, LineSplineLength(Other.LineSplineLength) {}

FLineProxy::FLineProxy(FLineProxy&& Other) noexcept
	: BusStopDistanceList(MoveTemp(Other.BusStopDistanceList))
	, LineSplineLength(Other.LineSplineLength) {}

FLineProxy& FLineProxy::operator=(const FLineProxy& Other)
{
	if (this == &Other)
		return *this;

	BusStopDistanceList = Other.BusStopDistanceList;
	LineSplineLength    = Other.LineSplineLength;
	return *this;
}

FLineProxy& FLineProxy::operator=(FLineProxy&& Other) noexcept
{
	if (this == &Other)
		return *this;

	BusStopDistanceList = MoveTemp(Other.BusStopDistanceList);
	LineSplineLength    = Other.LineSplineLength;
	return *this;
}


//~=============================================================================
// FStopProxy
FStopProxy::FStopProxy()
	: BusRoutes()
	, StopLocation(FVector2d::ZeroVector) {}

FStopProxy::FStopProxy(const TArray<TWeakObjectPtr<AOmniLineBusRoute>>& InBusRoutes, const FVector2D& InStopLocation)
	: BusRoutes(InBusRoutes)
	, StopLocation(InStopLocation) {}

FStopProxy::FStopProxy(TArray<TWeakObjectPtr<AOmniLineBusRoute>>&& InBusRoutes, FVector2D&& InStopLocation)
	: BusRoutes(MoveTemp(InBusRoutes))
	, StopLocation(MoveTemp(InStopLocation)) {}

FStopProxy::FStopProxy(const FStopProxy& Other)
	: BusRoutes(Other.BusRoutes)
	, StopLocation(Other.StopLocation) {}

FStopProxy::FStopProxy(FStopProxy&& Other) noexcept
	: BusRoutes(MoveTemp(Other.BusRoutes))
	, StopLocation(MoveTemp(Other.StopLocation)) {}

FStopProxy& FStopProxy::operator=(const FStopProxy& Other)
{
	if (this == &Other)
		return *this;

	BusRoutes    = Other.BusRoutes;
	StopLocation = Other.StopLocation;
	return *this;
}

FStopProxy& FStopProxy::operator=(FStopProxy&& Other) noexcept
{
	if (this == &Other)
		return *this;

	BusRoutes    = MoveTemp(Other.BusRoutes);
	StopLocation = MoveTemp(Other.StopLocation);
	return *this;
}


//~=============================================================================
// FOmniPathFindingQuery

FOmniPathFindingQuery::FOmniPathFindingQuery(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InStartList
                                           , const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InEndList
                                           , const TArray<FTransferRule>& InTransferRuleList
                                           , const FPathFindingQueryDelegate& InOnGameThreadDelegate)
	: QueryID(GetUniqueQueryID())
	, StartList(InStartList)
	, EndList(InEndList)
	, TransferRuleList(InTransferRuleList)
	, OnGameThreadDelegate(InOnGameThreadDelegate)
{}

FOmniPathFindingQuery::FOmniPathFindingQuery(const FOmniPathFindingQuery& Other)
	: QueryID(Other.QueryID)
	, StartList(Other.StartList)
	, EndList(Other.EndList)
	, TransferRuleList(Other.TransferRuleList)
	, OnGameThreadDelegate(Other.OnGameThreadDelegate)
{}

FOmniPathFindingQuery::FOmniPathFindingQuery(FOmniPathFindingQuery&& Other) noexcept
	: QueryID(Other.QueryID)
	, StartList(MoveTemp(Other.StartList))
	, EndList(MoveTemp(Other.EndList))
	, TransferRuleList(MoveTemp(Other.TransferRuleList))
	, OnGameThreadDelegate(MoveTemp(Other.OnGameThreadDelegate))
{}

FOmniPathFindingQuery& FOmniPathFindingQuery::operator=(const FOmniPathFindingQuery& Other)
{
	if (this == &Other)
		return *this;

	QueryID              = Other.QueryID;
	StartList            = Other.StartList;
	EndList              = Other.EndList;
	TransferRuleList     = Other.TransferRuleList;
	OnGameThreadDelegate = Other.OnGameThreadDelegate;
	return *this;
}

FOmniPathFindingQuery& FOmniPathFindingQuery::operator=(FOmniPathFindingQuery&& Other) noexcept
{
	if (this == &Other)
		return *this;

	QueryID              = Other.QueryID;
	StartList            = MoveTemp(Other.StartList);
	EndList              = MoveTemp(Other.EndList);
	TransferRuleList     = MoveTemp(Other.TransferRuleList);
	OnGameThreadDelegate = MoveTemp(Other.OnGameThreadDelegate);
	return *this;
}
