// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingChunk.h"

#include "ChunkManager.h"
#include "kkumigi/Voxel/Utils/FastNoiseLite.h"

#include "UTLStatics.h"

#include <array>

void AMarchingChunk::ReserveChunkVoxels()
{
	ChunkVoxels = std::vector<std::vector<std::vector<float>>>(ChunkSizeVec.Z + 1, std::vector<std::vector<float>>(ChunkSizeVec.Y + 1, std::vector<float>(ChunkSizeVec.X + 1, 0.0f)));
}

void AMarchingChunk::CalculateMeshData()
{
	// Triangulation order
	OrderTriangleVertex();

	FVertexTypeArray CubeVertices;

	for (int IdxX = 0; IdxX < ChunkSizeVec.X; ++IdxX)
	{
		for (int IdxY = 0; IdxY < ChunkSizeVec.Y; ++IdxY)
		{
			for (int IdxZ = 0; IdxZ < ChunkSizeVec.Z; ++IdxZ)
			{
				for (int VertexIdx = 0; VertexIdx < CubeVertices.size(); ++VertexIdx)
				{
					CubeVertices[VertexIdx] = ChunkVoxels[IdxZ + VertexOffset[VertexIdx][2]][IdxY + VertexOffset[VertexIdx][1]][IdxX + VertexOffset[VertexIdx][0]];
				}

				March(IdxX, IdxY, IdxZ, CubeVertices);
			}
		}
	}
}

void AMarchingChunk::March(const int InX, const int InY, const int InZ, const FVertexTypeArray& InCubeVertices)
{
	int InsideVertexMask = 0;

	std::array<FVector, 12> VertexOnEdge;     // 모서리 위에 있는 교차점 버텍스의 위치
	std::array<FIdxVector, 12> EdgeVertexIdx; // 교차하는 버텍스(모서리를 구성하는)의 인덱스. 삼각형의 종류를 찾을 때 사용.

	// 표면 내부에 있는 정점 찾기.
	for (int i = 0; i < InCubeVertices.size(); ++i)
	{
		if (GetBlockType(InCubeVertices[i]) != EBlockType::Air)
		{
			InsideVertexMask |= (1 << i);
		}
	}

	// vertex가 교차하는 모서리 목록
	const int CubeEdgeMask = CubeEdgeTable[InsideVertexMask];

	// vertex가 모두 밖에 있거나, 안에 있음.
	if (CubeEdgeMask == 0)
		return;

	// 모서리 위에 있는 교차점 찾기
	for (int i = 0; i < 12; ++i)
	{
		if ((CubeEdgeMask & 1 << i) != 0)
		{
			auto [BlockType1, Weight1] = GetBlockTypeAndWeight(InCubeVertices[EdgeVertexTable[i][0]]);
			auto [BlockType2, Weight2] = GetBlockTypeAndWeight(InCubeVertices[EdgeVertexTable[i][1]]);
			const float Offset         = Interpolation ? GetInterpolationOffset(Weight1, Weight2) : 0.5f;

			// vertex 사이에 있는 교차점(모서리 중간) 위치 찾기
			VertexOnEdge[i].X = InX + (VertexOffset[EdgeVertexTable[i][0]][0] + Offset * EdgeVertexOffset[i][0]);
			VertexOnEdge[i].Y = InY + (VertexOffset[EdgeVertexTable[i][0]][1] + Offset * EdgeVertexOffset[i][1]);
			VertexOnEdge[i].Z = InZ + (VertexOffset[EdgeVertexTable[i][0]][2] + Offset * EdgeVertexOffset[i][2]);

			EdgeVertexIdx[i].X = InX + VertexOffset[EdgeVertexTable[i][0]][0];
			EdgeVertexIdx[i].Y = InY + VertexOffset[EdgeVertexTable[i][0]][1];
			EdgeVertexIdx[i].Z = InZ + VertexOffset[EdgeVertexTable[i][0]][2];
		}
	}

	// 청크 각 변의 크기
	const int Max_X = ChunkVoxels.front().front().size() - 1;
	const int Max_Y = ChunkVoxels.front().size() - 1;
	const int Max_Z = ChunkVoxels.size() - 1;

	// Save triangles, at most can be 5
	// 삼각형 저장 최대 5개.
	for (int i = 0; i < 5; ++i)
	{
		if (TriTable[InsideVertexMask][3 * i] < 0)
			break;

		// Vertex
		const FVector V1_Relation = VertexOnEdge[TriTable[InsideVertexMask][3 * i]];
		const FVector V2_Relation = VertexOnEdge[TriTable[InsideVertexMask][3 * i + 1]];
		const FVector V3_Relation = VertexOnEdge[TriTable[InsideVertexMask][3 * i + 2]];

		// 큐브 위치와 틀어져서 생성되는 삼각형의 위치를 보정합니다.
		// todo: 너 나은 방법이 있을 수 있음
		const FVector V1 = V1_Relation * GetUnitVoxelLength() + GetUnitVoxelLength() / 2.0f;
		const FVector V2 = V2_Relation * GetUnitVoxelLength() + GetUnitVoxelLength() / 2.0f;;
		const FVector V3 = V3_Relation * GetUnitVoxelLength() + GetUnitVoxelLength() / 2.0f;;

		// Vertex Normal
		// 왼손 좌표계에서 노말벡터 방향을 고려한 외적
		const FVector TriCrossProduct = FVector::CrossProduct(V3 - V1, V2 - V1);
		const FVector FaceNormal = TriCrossProduct.GetSafeNormal();

		// 삼각형 중심 좌표
		// 표면 반대 방향(안쪽)으로 반 칸 이동해서 계산.
		const FVector TriangleCenter = ((V1_Relation + V2_Relation + V3_Relation) / 3.0) + (FaceNormal * -0.5);

		// 삼각형과 가장 가까운 블록의 idx
		FIdxVector NearBlockIdx;
		NearBlockIdx.X = FMath::Clamp(FMath::RoundToInt(TriangleCenter.X), 0, Max_X);
		NearBlockIdx.Y = FMath::Clamp(FMath::RoundToInt(TriangleCenter.Y), 0, Max_Y);
		NearBlockIdx.Z = FMath::Clamp(FMath::RoundToInt(TriangleCenter.Z), 0, Max_Z);

		// 가장 가까운 블록의 타입
		EBlockType BlockType = GetBlockType(NearBlockIdx);

		// Air라면 삼각형 vertex에서 가져오기.
		for (int TriIdx = 0; TriIdx < 3; ++TriIdx)
		{
			if (BlockType != EBlockType::Air)
			{
				break;
			}

			const FIdxVector EVIdx = EdgeVertexIdx[TriTable[InsideVertexMask][3 * i + TriIdx]];
			BlockType = GetBlockType(EVIdx);
		}

		// Texture array Index용 color
		FColor TextureIdxColor = GetTextureIndexColor(BlockType, FaceNormal);

		MeshData.Vertices.Append({V1, V2, V3});
		MeshData.Triangles.Append({MeshData.VertexCount + TriOrder[0], MeshData.VertexCount + TriOrder[1], MeshData.VertexCount + TriOrder[2]});
		MeshData.Colors.Append({TextureIdxColor, TextureIdxColor, TextureIdxColor});
		MeshData.UV0.Append({FVector2D(0.5, 0.0), FVector2D(0.0, 1.0), FVector2D(1.0, 1.0)});

		MeshData.Normals.Append({FaceNormal, FaceNormal, FaceNormal});

		// 삼각형의 면적을 가중치로 사용하기 위해, 정규화하지 않은 외적을 사용합니다.
		if (GetChunkManager()->IsUseSmoothingNormal())
		{
			// 부동소수점 오차 고려
			MeshData.NormalSums.FindOrAdd(UtlMath::RoundVector(V1, 0.001)) += TriCrossProduct;
			MeshData.NormalSums.FindOrAdd(UtlMath::RoundVector(V2, 0.001)) += TriCrossProduct;
			MeshData.NormalSums.FindOrAdd(UtlMath::RoundVector(V3, 0.001)) += TriCrossProduct;
		}

		MeshData.VertexCount += 3;
	}
}

void AMarchingChunk::AsyncGenerateMesh()
{
	ExecuteAsyncGenerateMesh(AsyncCalculateMeshData);
}

void AMarchingChunk::AsyncCalculateMeshData(const FAsyncChunkMeshData& InAsyncData, FChunkMeshData& OutMeshData)
{
	FVertexTypeArray CubeVertices;
	for (int IdxX = 0; IdxX < InAsyncData.InChunkSizeVec.X; ++IdxX)
	{
		for (int IdxY = 0; IdxY < InAsyncData.InChunkSizeVec.Y; ++IdxY)
		{
			for (int IdxZ = 0; IdxZ < InAsyncData.InChunkSizeVec.Z; ++IdxZ)
			{
				for (int VertexIdx = 0; VertexIdx < CubeVertices.size(); ++VertexIdx)
				{
					CubeVertices[VertexIdx] = InAsyncData.InChunkVoxels[IdxZ + VertexOffset[VertexIdx][2]][IdxY + VertexOffset[VertexIdx][1]][IdxX + VertexOffset[VertexIdx][0]];
				}

				AsyncMarch(IdxX, IdxY, IdxZ, CubeVertices, InAsyncData, OutMeshData);
			}
		}
	}
}

void AMarchingChunk::AsyncMarch(const int InX, const int InY, const int InZ, const FVertexTypeArray& InCubeVertices, const FAsyncChunkMeshData& InAsyncData, FChunkMeshData& OutMeshData)
{
	int InsideVertexMask = 0;

	std::array<FVector, 12> VertexOnEdge;     // 모서리 위에 있는 교차점 버텍스의 위치
	std::array<FIdxVector, 12> EdgeVertexIdx; // 교차하는 버텍스(모서리를 구성하는)의 인덱스. 삼각형의 종류를 찾을 때 사용.

	// 표면 내부에 있는 정점 찾기.
	for (int i = 0; i < InCubeVertices.size(); ++i)
	{
		if (GetBlockType_Static(InCubeVertices[i], InAsyncData.InSurfaceLevel) != EBlockType::Air)
		{
			InsideVertexMask |= (1 << i);
		}
	}

	// vertex가 교차하는 모서리 목록
	const int CubeEdgeMask = CubeEdgeTable[InsideVertexMask];

	// vertex가 모두 밖에 있거나, 안에 있음.
	if (CubeEdgeMask == 0)
		return;

	// 모서리 위에 있는 교차점 찾기
	for (int i = 0; i < 12; ++i)
	{
		if ((CubeEdgeMask & 1 << i) != 0)
		{
			const float Offset         =  0.5f;

			// vertex 사이에 있는 교차점(모서리 중간) 위치 찾기
			VertexOnEdge[i].X = InX + (VertexOffset[EdgeVertexTable[i][0]][0] + Offset * EdgeVertexOffset[i][0]);
			VertexOnEdge[i].Y = InY + (VertexOffset[EdgeVertexTable[i][0]][1] + Offset * EdgeVertexOffset[i][1]);
			VertexOnEdge[i].Z = InZ + (VertexOffset[EdgeVertexTable[i][0]][2] + Offset * EdgeVertexOffset[i][2]);

			EdgeVertexIdx[i].X = InX + VertexOffset[EdgeVertexTable[i][0]][0];
			EdgeVertexIdx[i].Y = InY + VertexOffset[EdgeVertexTable[i][0]][1];
			EdgeVertexIdx[i].Z = InZ + VertexOffset[EdgeVertexTable[i][0]][2];
		}
	}

	// 청크 각 변의 크기
	const int Max_X = InAsyncData.InChunkVoxels.front().front().size() - 1;
	const int Max_Y = InAsyncData.InChunkVoxels.front().size() - 1;
	const int Max_Z = InAsyncData.InChunkVoxels.size() - 1;

	// Save triangles, at most can be 5
	// 삼각형 저장 최대 5개.
	for (int i = 0; i < 5; ++i)
	{
		if (TriTable[InsideVertexMask][3 * i] < 0)
			break;

		// Vertex
		const FVector V1_Relation = VertexOnEdge[TriTable[InsideVertexMask][3 * i]];
		const FVector V2_Relation = VertexOnEdge[TriTable[InsideVertexMask][3 * i + 1]];
		const FVector V3_Relation = VertexOnEdge[TriTable[InsideVertexMask][3 * i + 2]];

		// 큐브 위치와 틀어져서 생성되는 삼각형의 위치를 보정합니다.
		// todo: 너 나은 방법이 있을 수 있음
		const FVector V1 = V1_Relation * InAsyncData.InUnitVoxelLength + InAsyncData.InUnitVoxelLength / 2.0f;
		const FVector V2 = V2_Relation * InAsyncData.InUnitVoxelLength + InAsyncData.InUnitVoxelLength / 2.0f;;
		const FVector V3 = V3_Relation * InAsyncData.InUnitVoxelLength + InAsyncData.InUnitVoxelLength / 2.0f;;

		// Vertex Normal
		// 왼손 좌표계에서 노말벡터 방향을 고려한 외적
		const FVector TriCrossProduct = FVector::CrossProduct(V3 - V1, V2 - V1);
		const FVector FaceNormal = TriCrossProduct.GetSafeNormal();

		// 삼각형 중심 좌표
		// 표면 반대 방향(안쪽)으로 반 칸 이동해서 계산.
		const FVector TriangleCenter = ((V1_Relation + V2_Relation + V3_Relation) / 3.0) + (FaceNormal * -0.5);

		// 삼각형과 가장 가까운 블록의 idx
		FIdxVector NearBlockIdx;
		NearBlockIdx.X = FMath::Clamp(FMath::RoundToInt(TriangleCenter.X), 0, Max_X);
		NearBlockIdx.Y = FMath::Clamp(FMath::RoundToInt(TriangleCenter.Y), 0, Max_Y);
		NearBlockIdx.Z = FMath::Clamp(FMath::RoundToInt(TriangleCenter.Z), 0, Max_Z);

		// 가장 가까운 블록의 타입
		EBlockType BlockType = GetBlockType_Static(NearBlockIdx, InAsyncData.InChunkVoxels, InAsyncData.InSurfaceLevel);

		// Air라면 삼각형 vertex에서 가져오기.
		for (int TriIdx = 0; TriIdx < 3; ++TriIdx)
		{
			if (BlockType != EBlockType::Air)
			{
				break;
			}

			const FIdxVector EVIdx = EdgeVertexIdx[TriTable[InsideVertexMask][3 * i + TriIdx]];
			BlockType = GetBlockType_Static(EVIdx, InAsyncData.InChunkVoxels, InAsyncData.InSurfaceLevel);
		}

		// Texture array Index용 color
		FColor TextureIdxColor = GetTextureIndexColor(BlockType, FaceNormal);

		OutMeshData.Vertices.Append({V1, V2, V3});
		OutMeshData.Triangles.Append({OutMeshData.VertexCount + 0, OutMeshData.VertexCount + 1, OutMeshData.VertexCount + 2});
		OutMeshData.Colors.Append({TextureIdxColor, TextureIdxColor, TextureIdxColor});
		OutMeshData.UV0.Append({FVector2D(0.5, 0.0), FVector2D(0.0, 1.0), FVector2D(1.0, 1.0)});

		OutMeshData.Normals.Append({FaceNormal, FaceNormal, FaceNormal});

		// 삼각형의 면적을 가중치로 사용하기 위해, 정규화하지 않은 외적을 사용합니다.
		if (InAsyncData.InbUseSmoothingNormal)
		{
			// 부동소수점 오차 고려
			OutMeshData.NormalSums.FindOrAdd(UtlMath::RoundVector(V1, 0.001)) += TriCrossProduct;
			OutMeshData.NormalSums.FindOrAdd(UtlMath::RoundVector(V2, 0.001)) += TriCrossProduct;
			OutMeshData.NormalSums.FindOrAdd(UtlMath::RoundVector(V3, 0.001)) += TriCrossProduct;
		}

		OutMeshData.VertexCount += 3;
	}
}


float AMarchingChunk::GetInterpolationOffset(const float InV1, const float InV2) const
{
	const float Delta = InV2 - InV1;
	return Delta == 0.0f ? SurfaceLevel : (SurfaceLevel - InV1) / Delta;
}

void AMarchingChunk::OrderTriangleVertex()
{
	if (SurfaceLevel >= 0.0f)
	{
		TriOrder[0] = 0;
		TriOrder[1] = 1;
		TriOrder[2] = 2;
	}
	else
	{
		TriOrder[0] = 2;
		TriOrder[1] = 1;
		TriOrder[2] = 0;
	}
}

