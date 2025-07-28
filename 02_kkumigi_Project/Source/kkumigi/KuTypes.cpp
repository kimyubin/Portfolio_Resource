// Fill out your copyright notice in the Description page of Project Settings.


#include "KuTypes.h"

#include "Voxel/Chunk/ChunkBase.h"
#include "Voxel/Chunk/ChunkManager.h"

const FName FKuCollisionProfile::FieldItem(TEXT("FieldItem"));
const FName FKuCollisionProfile::PreviewFieldItem(TEXT("PreviewFieldItem"));
const FName FKuCollisionProfile::LineTraceOnly(TEXT("LineTraceOnly"));
const FName FKuCollisionProfile::UITraceOnly(TEXT("UITraceOnly"));

const ECollisionChannel FKuCollisionChannel::FieldItemTrace = ECollisionChannel::ECC_GameTraceChannel3;


FItemTableRow::FItemTableRow(): ItemDefID(INDEX_NONE)
                              , ItemSignatureName() {}

FItemInvenSaveData::FItemInvenSaveData()
	: Col(0)
	, Row(0)
	, Items() {}

FItemInvenSaveData::FItemInvenSaveData(const uint32 InCol, const uint32 InRow, const FItemData& InItems)
	: Col(InCol)
	, Row(InRow)
	, Items(InItems) {}

FItemFieldSaveData::FItemFieldSaveData(): Location(0)
                                        , Rotator(0)
                                        , Items()
                                        , AttachedItems() {}


FItemFieldSaveData::FItemFieldSaveData(const FVector3d& InLocation, const FRotator3d& InRotator, const FItemData& InItems, const TArray<FItemFieldSaveData>& InAttachedItems)
	: Location(InLocation)
	, Rotator(InRotator)
	, Items(InItems)
	, AttachedItems(InAttachedItems) {}


FAsyncChunkMeshData::FAsyncChunkMeshData(const AChunkBase* InChunkBase)
{
	if (InChunkBase != nullptr)
	{
		InQueryID             = InChunkBase->GetMeshQueryID();
		InChunkIdx            = InChunkBase->ChunkPositionIdx;
		InChunkSizeVec        = InChunkBase->ChunkSizeVec;
		InChunkVoxels         = InChunkBase->ChunkVoxels;
		InUnitVoxelLength     = InChunkBase->GetUnitVoxelLength();
		InbUseSmoothingNormal = InChunkBase->GetChunkManager()->IsUseSmoothingNormal();
		InSurfaceLevel        = InChunkBase->SurfaceLevel;
	}
}

FAsyncChunkMeshData::FAsyncChunkMeshData(FAsyncChunkMeshData&& Other) noexcept
	: InQueryID(MoveTemp(Other.InQueryID))
	, InChunkIdx(MoveTemp(Other.InChunkIdx))
	, InChunkSizeVec(MoveTemp(Other.InChunkSizeVec))
	, InChunkVoxels(MoveTemp(Other.InChunkVoxels))
	, InUnitVoxelLength(Other.InUnitVoxelLength)
	, InbUseSmoothingNormal(Other.InbUseSmoothingNormal)
	, InSurfaceLevel(Other.InSurfaceLevel) {}

FAsyncChunkMeshData& FAsyncChunkMeshData::operator=(FAsyncChunkMeshData&& Other) noexcept
{
	if (this == &Other)
		return *this;

	InQueryID             = (MoveTemp(Other.InQueryID));
	InChunkIdx            = MoveTemp(Other.InChunkIdx);
	InChunkSizeVec        = MoveTemp(Other.InChunkSizeVec);
	InChunkVoxels         = MoveTemp(Other.InChunkVoxels);
	InUnitVoxelLength     = Other.InUnitVoxelLength;
	InbUseSmoothingNormal = Other.InbUseSmoothingNormal;
	InSurfaceLevel        = Other.InSurfaceLevel;
	return *this;
}
