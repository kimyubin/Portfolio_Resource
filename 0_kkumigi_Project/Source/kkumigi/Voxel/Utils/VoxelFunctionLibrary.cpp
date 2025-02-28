// Fill out your copyright notice in the Description page of Project Settings.


#include "kkumigi/Voxel/Utils/VoxelFunctionLibrary.h"

#include "UTLStatics.h"

TArray<FVector> UVoxelFunctionLibrary::SphereVoxelWorldPositions(const FVector& InCenterPos, const float InRadius, const float InVoxelLength)
{
	TArray<FVector> VoxelPositions;
	const float HalfVoxel     = InVoxelLength / 2.0f;
	const float RadiusSquared = InRadius * InRadius;

	const FIntVector MinPoint = UtlMath::FloorToIntVector((InCenterPos - InRadius), InVoxelLength);
	const FIntVector MaxPoint = UtlMath::CeilToIntVector((InCenterPos + InRadius), InVoxelLength);

	for (float x = MinPoint.X; x <= MaxPoint.X; x += InVoxelLength)
	{
		for (float y = MinPoint.Y; y <= MaxPoint.Y; y += InVoxelLength)
		{
			for (float z = MinPoint.Z; z <= MaxPoint.Z; z += InVoxelLength)
			{
				const FVector VoxelCenter = FVector(x, y, z) + HalfVoxel;
				if (FVector::DistSquared(VoxelCenter, InCenterPos) <= RadiusSquared)
				{
					VoxelPositions.Add(FVector(x, y, z));
				}
			}
		}
	}

	return VoxelPositions;
}

TArray<FVector> UVoxelFunctionLibrary::BoxVoxelWorldPositionsByLength(const FVector& InCenterPos, const float InLength, const float InVoxelLength)
{
	return BoxVoxelWorldPositionsByExtent(InCenterPos, FVector(InLength), InVoxelLength);
}

TArray<FVector> UVoxelFunctionLibrary::BoxVoxelWorldPositionsByExtent(const FVector& InCenterPos, const FVector& InExtent, const float InVoxelLength)
{
	const FVector HalfCube = InExtent;
	const FVector PointA   = InCenterPos - HalfCube;
	const FVector PointB   = InCenterPos + HalfCube;

	return BoxVoxelWorldPositions(PointA, PointB, InVoxelLength);
}

TArray<FVector> UVoxelFunctionLibrary::BoxVoxelWorldPositions(const FVector& InPointA, const FVector& InPointB, const float InVoxelLength)
{
	TArray<FVector> VoxelPositions;
	const FVector MinPoint = FVector(InPointA.X <= InPointB.X ? InPointA.X : InPointB.X
	                               , InPointA.Y <= InPointB.Y ? InPointA.Y : InPointB.Y
	                               , InPointA.Z <= InPointB.Z ? InPointA.Z : InPointB.Z);

	const FVector MaxPoint = FVector(InPointA.X > InPointB.X ? InPointA.X : InPointB.X
	                               , InPointA.Y > InPointB.Y ? InPointA.Y : InPointB.Y
	                               , InPointA.Z > InPointB.Z ? InPointA.Z : InPointB.Z);

	const FVector MinIntPoint = UtlMath::RoundVector(MinPoint, InVoxelLength);
	const FVector MaxIntPoint = UtlMath::RoundVector(MaxPoint, InVoxelLength);

	for (float x = MinIntPoint.X; x <= MaxIntPoint.X; x += InVoxelLength)
	{
		for (float y = MinIntPoint.Y; y <= MaxIntPoint.Y; y += InVoxelLength)
		{
			for (float z = MinIntPoint.Z; z <= MaxIntPoint.Z; z += InVoxelLength)
			{
				VoxelPositions.Add(FVector(x, y, z));
			}
		}
	}

	return VoxelPositions;
}
