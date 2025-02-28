// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveChunk.h"

#include "kkumigi/Voxel/Utils/FastNoiseLite.h"
#include "UtlLog.h"

#include <array>


std::array<FVector, 8> ANaiveChunk::BlockVertexData = {
	FVector(1, 1, 1)
  , FVector(1, 0, 1)
  , FVector(1, 0, 0)
  , FVector(1, 1, 0)
  , FVector(0, 0, 1)
  , FVector(0, 1, 1)
  , FVector(0, 1, 0)
  , FVector(0, 0, 0)
};

void ANaiveChunk::ReserveChunkVoxels()
{
	ChunkVoxels = std::vector<std::vector<std::vector<float>>>(ChunkSizeVec.Z, std::vector<std::vector<float>>(ChunkSizeVec.Y, std::vector<float>(ChunkSizeVec.X, 0.0f)));

	// static 변수를 각 객체에서 매번 초기화 되는 거 방지.
	if (FMath::IsNearlyEqual(BlockVertexData.front().X, GetUnitVoxelLength()) == false)
	{
		for (FVector& BVData : ANaiveChunk::BlockVertexData)
		{
			BVData *= GetUnitVoxelLength();
		}
	}
}

void ANaiveChunk::CalculateMeshData()
{
	for (int IdxX = 0; IdxX < ChunkSizeVec.X; ++IdxX)
	{
		for (int IdxY = 0; IdxY < ChunkSizeVec.Y; ++IdxY)
		{
			for (int IdxZ = 0; IdxZ < ChunkSizeVec.Z; ++IdxZ)
			{
				if (GetBlockType(ChunkVoxels[IdxZ][IdxY][IdxX]) != EBlockType::Air)
				{
					const FIdxVector VoxelIdx = FIdxVector(IdxX, IdxY, IdxZ);

					// 현재 블록 주변의 인접 6개 블록을 확인 후, 인접 블록이 air면 face 생성
					for (const EVXDirection Direction : {EVXDirection::Forward, EVXDirection::Right, EVXDirection::Back, EVXDirection::Left, EVXDirection::Up, EVXDirection::Down})
					{
						// 외부 경계에서만 face 생성
						if (IsOuterBlockAir(GetPositionInDirection(Direction, VoxelIdx)))
						{
							CreateFace(Direction, VoxelIdx);
						}
					}
				}
			}
		}
	}
}

bool ANaiveChunk::IsOuterBlockAir(const FIdxVector& InVoxelIdx) const
{
	if (IsOutOfChunkBoundary(InVoxelIdx))
		return true;

	return GetBlockType(InVoxelIdx) == EBlockType::Air;
}

void ANaiveChunk::CreateFace(const EVXDirection InDirection, const FIdxVector& InVoxelIdx)
{
	const FVector FacePos = FVector(InVoxelIdx) * GetUnitVoxelLength();
	const FVector Normal  = GetNormal(InDirection);
	const FColor Color    = GetTextureIndexColor(GetBlockType(InVoxelIdx), Normal);

	MeshData.Vertices.Append(GetFaceVertices(InDirection, FacePos));
	MeshData.Triangles.Append({MeshData.VertexCount + 3, MeshData.VertexCount + 2, MeshData.VertexCount, MeshData.VertexCount + 2, MeshData.VertexCount + 1, MeshData.VertexCount});
	MeshData.Normals.Append({Normal, Normal, Normal, Normal});
	MeshData.Colors.Append({Color, Color, Color, Color});
	MeshData.UV0.Append({FVector2D(0, 0), FVector2D(1, 0), FVector2D(1, 1), FVector2D(0, 1)});

	// 한면이 추가될 때마다 버텍스 수가 4개 증가함.
	MeshData.VertexCount += 4;
}

TArray<FVector> ANaiveChunk::GetFaceVertices(const EVXDirection InDirection, const FVector& InPosition) const
{
	TArray<FVector> Vertices;

	for (int idx = 0; idx < 4; ++idx)
	{
		// BlockTriangleData - 페이스의 방향에 따른 정점 위치에 대한 인덱스
		// BlockVertexData - 정점인덱스를 이곳에 적용해서 각 정점의 위치를 찾아냄. 
		Vertices.Add(BlockVertexData[BlockTriangleData[idx + static_cast<int>(InDirection) * 4]] + InPosition);
	}
	
	return Vertices;
}

FIdxVector ANaiveChunk::GetPositionInDirection(const EVXDirection InDirection, const FIdxVector& InVoxelIdx)
{
	FVector Direction;
	switch (InDirection)
	{
		case EVXDirection::Forward:
			Direction = FVector::ForwardVector;
			break;
		case EVXDirection::Back:
			Direction = FVector::BackwardVector;
			break;
		case EVXDirection::Left:
			Direction = FVector::LeftVector;
			break;
		case EVXDirection::Right:
			Direction = FVector::RightVector;
			break;
		case EVXDirection::Up:
			Direction = FVector::UpVector;
			break;
		case EVXDirection::Down:
			Direction = FVector::DownVector;
			break;
		default:
			UT_LOG("Invalid direction")
			Direction = FVector::ZeroVector;
			break;
	}
	return InVoxelIdx + FIdxVector(FIntVector(Direction));
}

FVector ANaiveChunk::GetNormal(const EVXDirection InDirection) const
{
	switch (InDirection)
	{
		case EVXDirection::Forward:
			return FVector::ForwardVector;
		case EVXDirection::Right:
			return FVector::RightVector;
		case EVXDirection::Back:
			return FVector::BackwardVector;
		case EVXDirection::Left:
			return FVector::LeftVector;
		case EVXDirection::Up:
			return FVector::UpVector;
		case EVXDirection::Down:
			return FVector::DownVector;
		default:
			UT_LOG("Invalid direction")
			return FVector::ZeroVector;
	}
}
