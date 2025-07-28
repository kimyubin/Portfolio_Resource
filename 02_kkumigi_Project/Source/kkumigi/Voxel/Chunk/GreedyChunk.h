// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ChunkBase.h"
#include "kkumigi/Voxel/Utils/Enums.h"

#include "GreedyChunk.generated.h"

class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class AGreedyChunk final : public AChunkBase
{
	GENERATED_BODY()

	struct FMask
	{
		EBlockType BlockType;
		int Normal;
	};

protected:
	virtual void ReserveChunkVoxels() override;
	virtual void CalculateMeshData() override;

private:
	void CreateQuad(const FMask InMask, const FIdxVector& InAxisMask, const int InWidth, const int InHeight, const FIdxVector& InV1, const FIdxVector& InV2, const FIdxVector& InV3, const FIdxVector& InV4);

	// ~========================
	// 비동기 버전
protected:
	virtual void AsyncGenerateMesh() override;
	static void AsyncCalculateMeshData(const FAsyncChunkMeshData& InAsyncData, FChunkMeshData& OutMeshData);

private:
	static void CreateQuad_Static(const FMask InMask, const FIdxVector& InAxisMask, const int InWidth, const int InHeight, const FIdxVector& InV1, const FIdxVector& InV2, const FIdxVector& InV3, const FIdxVector& InV4, const float InUnitVoxelLength, FChunkMeshData& OutMeshData);
	// ~========================

	static bool CompareMask(const FMask InM1, const FMask InM2);
};
