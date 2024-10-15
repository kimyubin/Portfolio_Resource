// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniAsyncTypes.h"

#include "OmniPassenger.h"

TQueryID FOmniPathFindingQuery::LastPathQueryUniqueID = INVALID_QUERY_ID;


//~=============================================================================
// FLineData
FLineData::FLineData()
	: BusStopDistanceList()
	, LineSplineLength(0.0f) {}

FLineData::FLineData(const TArray<FBusStopDistance>& InBusStopDistanceList, const float InLineSplineLength)
	: BusStopDistanceList(InBusStopDistanceList)
	, LineSplineLength(InLineSplineLength) {}

FLineData::FLineData(TArray<FBusStopDistance>&& InBusStopDistanceList, const float InLineSplineLength)
	: BusStopDistanceList(MoveTemp(InBusStopDistanceList))
	, LineSplineLength(InLineSplineLength) {}

FLineData::FLineData(const FLineData& Other)
	: BusStopDistanceList(Other.BusStopDistanceList)
	, LineSplineLength(Other.LineSplineLength) {}

FLineData::FLineData(FLineData&& Other) noexcept
	: BusStopDistanceList(MoveTemp(Other.BusStopDistanceList))
	, LineSplineLength(Other.LineSplineLength) {}

FLineData& FLineData::operator=(const FLineData& Other)
{
	if (this == &Other)
		return *this;

	BusStopDistanceList = Other.BusStopDistanceList;
	LineSplineLength    = Other.LineSplineLength;
	return *this;
}

FLineData& FLineData::operator=(FLineData&& Other) noexcept
{
	if (this == &Other)
		return *this;

	BusStopDistanceList = MoveTemp(Other.BusStopDistanceList);
	LineSplineLength    = Other.LineSplineLength;
	return *this;
}


//~=============================================================================
// FStopData
FStopData::FStopData()
	: BusRoutes()
	, StopLocation(FVector2d::ZeroVector) {}

FStopData::FStopData(const TArray<TWeakObjectPtr<AOmniLineBusRoute>>& InBusRoutes, const FVector2D& InStopLocation)
	: BusRoutes(InBusRoutes)
	, StopLocation(InStopLocation) {}

FStopData::FStopData(TArray<TWeakObjectPtr<AOmniLineBusRoute>>&& InBusRoutes, FVector2D&& InStopLocation)
	: BusRoutes(MoveTemp(InBusRoutes))
	, StopLocation(MoveTemp(InStopLocation)) {}

FStopData::FStopData(const FStopData& Other)
	: BusRoutes(Other.BusRoutes)
	, StopLocation(Other.StopLocation) {}

FStopData::FStopData(FStopData&& Other) noexcept
	: BusRoutes(MoveTemp(Other.BusRoutes))
	, StopLocation(MoveTemp(Other.StopLocation)) {}

FStopData& FStopData::operator=(const FStopData& Other)
{
	if (this == &Other)
		return *this;

	BusRoutes    = Other.BusRoutes;
	StopLocation = Other.StopLocation;
	return *this;
}

FStopData& FStopData::operator=(FStopData&& Other) noexcept
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
