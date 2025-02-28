#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "ChunkMeshData.generated.h"

USTRUCT()
struct FChunkMeshData
{
	GENERATED_BODY()

public:
	FChunkMeshData();
	FChunkMeshData(const FChunkMeshData& Other);
	FChunkMeshData(FChunkMeshData&& Other) noexcept;
	FChunkMeshData& operator=(const FChunkMeshData& Other);
	FChunkMeshData& operator=(FChunkMeshData&& Other) noexcept;

	void Clear();

	TArray<FVector> Vertices;

	/** 각 삼각형을 구성하는 정점을 나타내는 인덱스 버퍼입니다. 버텍스의 인덱스로 나타냅니다. */
	TArray<int> Triangles;
	TArray<FVector> Normals;

	/**
	 * 버텍스 인접 삼각형의 노말 누적값입니다.
	 * 평균값 혹은 가중평균값으로 버텍스의 노말을 구하는데 사용됩니다.
	 */
	TMap<FVector, FVector> NormalSums; // <Vertex, Normal>
	TArray<FColor> Colors;
	TArray<FVector2D> UV0;

	int VertexCount = 0;
	
	UE::Geometry::FDynamicMesh3 DynamicMesh3;

	FProcMeshSection NewMeshSection;
};

inline FChunkMeshData::FChunkMeshData()
{
	Clear();
}

inline FChunkMeshData::FChunkMeshData(const FChunkMeshData& Other)
	: Vertices(Other.Vertices)
	, Triangles(Other.Triangles)
	, Normals(Other.Normals)
	, NormalSums(Other.NormalSums)
	, Colors(Other.Colors)
	, UV0(Other.UV0)
	, VertexCount(Other.VertexCount)
	, DynamicMesh3(Other.DynamicMesh3)
	, NewMeshSection(Other.NewMeshSection) {}

inline FChunkMeshData::FChunkMeshData(FChunkMeshData&& Other) noexcept
	: Vertices(MoveTemp(Other.Vertices))
	, Triangles(MoveTemp(Other.Triangles))
	, Normals(MoveTemp(Other.Normals))
	, NormalSums(MoveTemp(Other.NormalSums))
	, Colors(MoveTemp(Other.Colors))
	, UV0(MoveTemp(Other.UV0))
	, VertexCount(Other.VertexCount)
	, DynamicMesh3(Other.DynamicMesh3)
	, NewMeshSection(Other.NewMeshSection) {}

inline FChunkMeshData& FChunkMeshData::operator=(const FChunkMeshData& Other)
{
	if (this == &Other)
		return *this;

	Vertices       = Other.Vertices;
	Triangles      = Other.Triangles;
	Normals        = Other.Normals;
	NormalSums     = Other.NormalSums;
	Colors         = Other.Colors;
	UV0            = Other.UV0;
	VertexCount    = Other.VertexCount;
	DynamicMesh3   = Other.DynamicMesh3;
	NewMeshSection = Other.NewMeshSection;
	return *this;
}

inline FChunkMeshData& FChunkMeshData::operator=(FChunkMeshData&& Other) noexcept
{
	if (this == &Other)
		return *this;

	Vertices       = MoveTemp(Other.Vertices);
	Triangles      = MoveTemp(Other.Triangles);
	Normals        = MoveTemp(Other.Normals);
	NormalSums     = MoveTemp(Other.NormalSums);
	Colors         = MoveTemp(Other.Colors);
	UV0            = MoveTemp(Other.UV0);
	VertexCount    = Other.VertexCount;
	DynamicMesh3   = MoveTemp(Other.DynamicMesh3);
	NewMeshSection = MoveTemp(Other.NewMeshSection);
	return *this;
}

inline void FChunkMeshData::Clear()
{
	// 메시를 재구성할 때, 이전과 비슷한 메모리 공간을
	// 사용할 가능성이 크므로 Clear 대신 Reset 사용.
	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	NormalSums.Reset();
	Colors.Reset();
	UV0.Reset();

	VertexCount = 0;

	DynamicMesh3.Clear();
	NewMeshSection.Reset();
}
