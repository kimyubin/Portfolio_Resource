// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusPlayData.h"

#include "OmnibusUtilities.h"

UOmnibusPlayData::UOmnibusPlayData()
{
	CurrentLocation = FVector3d();
	PlayMode        = EOmniPlayMode::Move;
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
