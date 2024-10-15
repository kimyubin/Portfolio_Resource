// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusPlayData.h"

#include "OmnibusUtilities.h"
#include "OmniLineBusRoute.h"
#include "OmniPassenger.h"
#include "OmniVehicleBus.h"
#include "PathVisualizator.h"

UOmnibusPlayData::UOmnibusPlayData()
{
	PlayMode              = EOmniPlayMode::Move;
	OmniVehicleBusClass   = AOmniVehicleBus::StaticClass();
	OmniLineBusRouteClass = AOmniLineBusRoute::StaticClass();
	PathVisualizatorClass = APathVisualizator::StaticClass();
	PassengerClass        = AOmniPassenger::StaticClass();

	CityBlockIcons.Empty(EnumToInt(ECityBlockType::Size));
	for (int idx = EnumToInt(ECityBlockType::None); idx < EnumToInt(ECityBlockType::Size); ++idx)
	{
		CityBlockIcons.Emplace(static_cast<ECityBlockType>(idx), nullptr);
	}

	BusLineMesh      = nullptr;
	PathVisualMesh   = nullptr;
	CarSpecDataTable = nullptr;

	BaseBusSpeed        = 900.0f;
	BaseBusAcceleration = 3000.0f;

	TransferRuleList = {FTransferRule(200.0f), FTransferRule(300.0f), FTransferRule(400.0f), FTransferRule(500.0f)};
}

void UOmnibusPlayData::Initialize()
{
	ReadCarSpecSheet();
}

UStaticMesh* UOmnibusPlayData::GetPlainRoadMesh(const int& InMeshIdx) const
{
	if (RoadMeshSets.IsValidIndex(InMeshIdx) && OB_IS_VALID(RoadMeshSets[InMeshIdx].PlainRoadMesh))
		return RoadMeshSets[InMeshIdx].PlainRoadMesh;

	return nullptr;
}

UStaticMesh* UOmnibusPlayData::GetIntersectionFlat4WayRoadMesh(const int& InMeshIdx) const
{
	if (RoadMeshSets.IsValidIndex(InMeshIdx) && OB_IS_VALID(RoadMeshSets[InMeshIdx].IntersectionFlat4WayRoadMesh))
		return RoadMeshSets[InMeshIdx].IntersectionFlat4WayRoadMesh;

	return nullptr;
}

UTexture2D* UOmnibusPlayData::GetCityBlockIcon(const ECityBlockType InCityBlockType) const
{
	if (CityBlockIcons.IsValidIndex(EnumToInt(InCityBlockType)))
		return CityBlockIcons[EnumToInt(InCityBlockType)].IconTexture;

	return nullptr;
}

void UOmnibusPlayData::ReadCarSpecSheet()
{
	if (OB_IS_VALID(CarSpecDataTable))
	{
		CarSpecList.Reset();

		static const FString ContextString(TEXT("UOmnibusPlayData::ReadCarSpecSheet"));
		TArray<FCarSpecRow*> CarSpecTableRows;
		CarSpecDataTable->GetAllRows<FCarSpecRow>(ContextString, CarSpecTableRows);
		for (FCarSpecRow* CarSpecRow : CarSpecTableRows)
		{
			CarSpecList.Emplace(CarSpecRow->CarType, FCarSpec{*CarSpecRow, BaseBusSpeed, BaseBusAcceleration});
		}
	}
}

FCarSpec UOmnibusPlayData::GetCarSpecList(const ECarType InCarType)
{
	const FCarSpec* FindPtr = CarSpecList.Find(InCarType);
	return FindPtr ? *FindPtr : FCarSpec();
}
