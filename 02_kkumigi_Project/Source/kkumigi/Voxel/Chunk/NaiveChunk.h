// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ChunkBase.h"
#include "kkumigi/Voxel/Utils/Enums.h"

#include "NaiveChunk.generated.h"

class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class ANaiveChunk final : public AChunkBase
{
	GENERATED_BODY()

protected:
	virtual void ReserveChunkVoxels() override;

	virtual void CalculateMeshData() override;

private:
	/**
	 * 큐브와 외부 경계를 확인합니다.
	 * @param InVoxelIdx 확인할 위치
	 * @return 외부와의 경계이거나 큐브가 air인 경우 true;
	 */
	bool IsOuterBlockAir(const FIdxVector& InVoxelIdx) const;
	void CreateFace(const EVXDirection InDirection, const FIdxVector& InVoxelIdx);

	/**
	 * 페이스의 정점을 방향과 위치로 계산합니다.
	 * 
	 * @param InDirection 페이스가 바라보는 방향 
	 * @param InPosition 블록의 현재 위치
	 * @return 페이스를 구성하는 정점들의 위치
	 */
	TArray<FVector> GetFaceVertices(const EVXDirection InDirection, const FVector& InPosition) const;

	/**
	 * 페이스 방향에 따른 위치를 반환합니다.
	 * 
	 * @param InDirection 
	 * @param InVoxelIdx 
	 * @return 
	 */
	static FIdxVector GetPositionInDirection(const EVXDirection InDirection, const FIdxVector& InVoxelIdx);
	FVector GetNormal(const EVXDirection InDirection) const;

	// 각 정점의 위치
	static std::array<FVector, 8> BlockVertexData;

	// 육면체 각 페이스를 구성하는 정점 위치(BlockVertexData)를 구하기 위한 인덱스
	// 정점의 위치는 인접한 면끼리 중복되기 때문에 인덱스로 구성함.
	static constexpr int BlockTriangleData[24] = {
		0, 1, 2, 3 // Forward
	  , 5, 0, 3, 6 // Right
	  , 4, 5, 6, 7 // Back
	  , 1, 4, 7, 2 // Left
	  , 5, 4, 1, 0 // Up
	  , 3, 2, 7, 6 // Down
	};
};
