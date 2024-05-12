// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusPlayData.h"

#include "OmnibusUtilities.h"
#include "OmniLineBusRoute.h"
#include "OmniVehicleBus.h"

UOmnibusPlayData::UOmnibusPlayData()
{
	CurrentLocation       = FVector3d();
	PlayMode              = EOmniPlayMode::Move;
	OmniVehicleBusClass   = AOmniVehicleBus::StaticClass();
	OmniLineBusRouteClass = AOmniLineBusRoute::StaticClass();

	CityBlockIcons.Empty(EnumToInt(ECityBlockType::Size));
	for (int idx = EnumToInt(ECityBlockType::None); idx < EnumToInt(ECityBlockType::Size); ++idx)
	{
		CityBlockIcons.Emplace(static_cast<ECityBlockType>(idx), nullptr);
	}
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

UTexture2D* UOmnibusPlayData::GetCityBlockIcon(ECityBlockType InCityBlockType) const
{
	if (CityBlockIcons.IsValidIndex(EnumToInt(InCityBlockType)))
		return CityBlockIcons[EnumToInt(InCityBlockType)].IconTexture;

	return nullptr;
}
