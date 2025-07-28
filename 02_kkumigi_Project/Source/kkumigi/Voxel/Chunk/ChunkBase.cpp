// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkBase.h"

#include "ChunkManager.h"
#include "KismetProceduralMeshLibrary.h"
#include "kkumigi/Voxel/Utils/FastNoiseLite.h"
#include "ProceduralMeshComponent.h"
#include "Components/DynamicMeshComponent.h"
#include "DynamicMesh/MeshAttributeUtil.h"
#include "kkumigi/AsyncProceduralMeshComponent.h"
#include "kkumigi/KuPlayData.h"
#include "kkumigi/Voxel/World/ChunkWorld.h"

#include "UtlLog.h"
#include "UTLStatics.h"


// Sets default values
AChunkBase::AChunkBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	AsyncProceduralMesh = CreateDefaultSubobject<UAsyncProceduralMeshComponent>("AsyncProceduralMesh");
	AsyncProceduralMesh->SetCastShadow(true);
	AsyncProceduralMesh->SetupAttachment(RootComponent);
	AsyncProceduralMesh->SetBoundsScale(10.0f);

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetCastShadow(true);
	ProceduralMesh->SetupAttachment(RootComponent);
	ProceduralMesh->SetBoundsScale(10.0f);

	DynamicMesh = CreateDefaultSubobject<UDynamicMeshComponent>("DynamicMesh");
	DynamicMesh->SetCastShadow(true);
	DynamicMesh->SetupAttachment(RootComponent);
	DynamicMesh->SetBoundsScale(10.0f);
	DynamicMesh->EnableComplexAsSimpleCollision();
	DynamicMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DynamicMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	ChunkSizeVec = FIdxVector(32);
	SurfaceLevel = 0.0f;

	bModifiedData = false;
}

void AChunkBase::InitializeChunk(const FIntVector& InChunkPosIdx, const FIdxVector& InChunkSize, UMaterialInterface* InMaterial)
{
	ChunkPositionIdx = InChunkPosIdx;
	ChunkSizeVec     = InChunkSize;
	Material         = InMaterial;

	ReserveChunkVoxels();
	InitializeChunkData(GetActorLocation() / GetUnitVoxelLength());
	CreateChunkMesh();
}

void AChunkBase::ReplaceChunk(const FIntVector& InChunkPosIdx, const FIdxVector& InChunkSize, UMaterialInterface* InMaterial)
{
	if (ChunkPositionIdx == InChunkPosIdx
		&& ChunkMeshIdx == InChunkPosIdx
		&& ChunkSizeVec == InChunkSize
		&& Material == InMaterial)
	{
		RootComponent->SetVisibility(true, true);
		return;
	}

	ChunkPositionIdx = InChunkPosIdx;
	ChunkSizeVec     = InChunkSize;
	Material         = InMaterial;

	InitializeChunkData(GetActorLocation() / GetUnitVoxelLength());
	CreateChunkMesh();
}

void AChunkBase::WaitChunk()
{
	ResetMeshQueryID();
	RootComponent->SetVisibility(false, true);
	SaveChunkData();
}

void AChunkBase::ReserveChunkVoxels()
{
	ChunkVoxels = std::vector<std::vector<std::vector<float>>>(ChunkSizeVec.Z + 1, std::vector<std::vector<float>>(ChunkSizeVec.Y + 1, std::vector<float>(ChunkSizeVec.X + 1, 0.0f)));
}

// Called when the game starts or when spawned
void AChunkBase::BeginPlay()
{
	Super::BeginPlay();

}

void AChunkBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bModifiedData)
	{
		SaveChunkData();
	}
	Super::EndPlay(EndPlayReason);
}

void AChunkBase::ModifyChunk(const TArray<FIdxVector>& InVoxelList, const EBlockType InBlock)
{
	for (const FIdxVector& VoxelIdx: InVoxelList)
	{
		ModifyVoxelData(VoxelIdx, InBlock);		
	}
	bModifiedData = true;

	CreateChunkMesh();
}

void AChunkBase::ModifyVoxelData(const FIdxVector& InVoxelIdx, const EBlockType InBlock)
{
	ModifyVoxelData(InVoxelIdx, GetNoiseFromBlockType(InBlock));
}

void AChunkBase::ModifyVoxelData(const FIdxVector& InVoxelIdx, const float InNoise)
{
	if (IsOutOfChunkBoundary(InVoxelIdx))
		return;

	ChunkVoxels[InVoxelIdx.Z][InVoxelIdx.Y][InVoxelIdx.X] = InNoise;
}

void AChunkBase::SaveChunkData()
{
	if (bModifiedData)
	{
		bModifiedData = false;
	}
}

void AChunkBase::LoadChunkData()
{
	
}

void AChunkBase::InitializeChunkData(const FVector& InPosition)
{
	// ChunkSizeVec 보다 배열이 클 수 있기 때문에
	// ChunkSizeVec 대신 실제 ChunkVoxels의 사이즈로 접근해야합니다.
	const int ChunkSize_X = ChunkVoxels.front().front().size();
	const int ChunkSize_Y = ChunkVoxels.front().size();
	const int ChunkSize_Z = ChunkVoxels.size();

	const int Height = GetSeaLevel();
	for (int IdxZ = 0; IdxZ < ChunkSize_Z; ++IdxZ)
	{
		for (int IdxY = 0; IdxY < ChunkSize_Y; ++IdxY)
		{
			for (int IdxX = 0; IdxX < ChunkSize_X; ++IdxX)
			{
				if (IdxZ > Get2DHeight(IdxX + InPosition.X, IdxY + InPosition.Y))
				// 기준 높이 이상이면, 공중으로 취급
				// if (IdxZ > Height)
				{
					ChunkVoxels[IdxZ][IdxY][IdxX] = GetNoiseFromBlockType(EBlockType::Air);
				}
				else
				{
					ChunkVoxels[IdxZ][IdxY][IdxX] = GetFastNoiseByIdx(IdxX + InPosition.X, IdxY + InPosition.Y, IdxZ + InPosition.Z);
				}
			}
		}
	}
}

void AChunkBase::CreateChunkMesh()
{
	AsyncProceduralMesh->SetMaterial(0, Material);
	ProceduralMesh->SetMaterial(0, Material);
	DynamicMesh->SetMaterial(0, Material);

	ClearMesh();

	if (GetChunkManager()->IsUsedAsyncCalculateMeshData())
	{
		AsyncGenerateMesh();
	}
	else
	{
		CalculateMeshData();

		ApplyMesh();
	}
}

bool CopyVertexColorToOverlay(const FDynamicMesh3& Mesh
                            , UE::Geometry::FDynamicMeshColorOverlay& ColorOverlayOut
                            , bool bCompactElements = false)
{
	if (!Mesh.HasVertexColors())
	{
		return false;
	}

	if (ColorOverlayOut.ElementCount() > 0)
	{
		ColorOverlayOut.ClearElements();
	}

	ColorOverlayOut.BeginUnsafeElementsInsert();
	for (int32 Vid : Mesh.VertexIndicesItr())
	{
		FVector3f Color = Mesh.GetVertexColor(Vid);
		ColorOverlayOut.InsertElement(Vid, &Color.X, true);
	}
	ColorOverlayOut.EndUnsafeElementsInsert();

	for (int32 Tid : Mesh.TriangleIndicesItr())
	{
		ColorOverlayOut.SetTriangle(Tid, Mesh.GetTriangle(Tid));
	}

	if (bCompactElements)
	{
		UE::Geometry::FCompactMaps CompactMaps;
		ColorOverlayOut.CompactInPlace(CompactMaps);
	}

	return true;
}

void AChunkBase::ExecuteAsyncGenerateMesh(TFunction<void(const FAsyncChunkMeshData& InAsyncData, FChunkMeshData& OutMeshData)>&& InCalculateMeshDataFunc)
{
	SetMeshQueryID();

	AsyncTask(ENamedThreads::AnyThread, [ ThisChunkWeak = TWeakObjectPtr<AChunkBase>(this), AsyncData = FAsyncChunkMeshData{this}, CalculateMeshDataFunc = MoveTemp(InCalculateMeshDataFunc) ]() mutable
	{
		// 메시 데이터 생성
		FChunkMeshData TempMeshData;
		CalculateMeshDataFunc(AsyncData, TempMeshData);

		// Normal스무딩 계산
		if (AsyncData.InbUseSmoothingNormal && TempMeshData.NormalSums.IsEmpty() == false)
		{
			TempMeshData.Normals.Reset();

			// 각 버텍스에 인접한 삼각형 노말의 누적합으로 버텍스 노말을 계산
			for (const FVector& Vtx : TempMeshData.Vertices)
			{
				FVector Vertex = UtlMath::RoundVector(Vtx, 0.001);
				TempMeshData.Normals.Add(TempMeshData.NormalSums[Vertex].GetSafeNormal());
			}
		}
		TArray<FVector2D> EmptyArray;

		CreateProcMeshSection(0
		                    , TempMeshData.Vertices
		                    , TempMeshData.Triangles
		                    , TempMeshData.Normals
		                    , TempMeshData.UV0
		                    , EmptyArray
		                    , EmptyArray
		                    , EmptyArray
		                    , TempMeshData.Colors
		                    , TArray<FProcMeshTangent>()
		                    , true
		                    , TempMeshData.NewMeshSection);
		

		// DynamicMesh 계산 
		TempMeshData.DynamicMesh3 = UE::Geometry::FDynamicMesh3(true, true, true, true);
		for (int idx = 0; idx < TempMeshData.Vertices.Num(); ++idx)
		{
			TempMeshData.DynamicMesh3.AppendVertex(UE::Geometry::FVertexInfo(TempMeshData.Vertices[idx]
			                                                               , FVector3f(TempMeshData.Normals[idx])
			                                                               , FVector3f(static_cast<float>(TempMeshData.Colors[idx].R) / 255.0f)
			                                                               , FVector2f(TempMeshData.UV0[idx])));
		}
		for (int idx = 0; idx < TempMeshData.Triangles.Num(); idx += 3)
		{
			TempMeshData.DynamicMesh3.AppendTriangle({TempMeshData.Triangles[idx], TempMeshData.Triangles[idx + 1], TempMeshData.Triangles[idx + 2]});
		}

		TempMeshData.DynamicMesh3.EnableAttributes();
		TempMeshData.DynamicMesh3.Attributes()->EnablePrimaryColors();

		UE::Geometry::CopyVertexUVsToOverlay(TempMeshData.DynamicMesh3, *TempMeshData.DynamicMesh3.Attributes()->PrimaryUV());
		UE::Geometry::CopyVertexNormalsToOverlay(TempMeshData.DynamicMesh3, *TempMeshData.DynamicMesh3.Attributes()->PrimaryNormals());
		CopyVertexColorToOverlay(TempMeshData.DynamicMesh3, *TempMeshData.DynamicMesh3.Attributes()->PrimaryColors());


		AsyncTask(ENamedThreads::GameThread, [MovedMeshData = MoveTemp(TempMeshData), ThisChunkWeak, AsyncQueryID = AsyncData.InQueryID ,ChunkIdx = AsyncData.InChunkIdx]() mutable
		{
			if (AChunkBase* ChunkBase = ThisChunkWeak.Get())
			{
				// 동일해야 적용
				if (AsyncQueryID == ChunkBase->MeshQueryID && ChunkIdx == ChunkBase->ChunkPositionIdx)
				{
					ChunkBase->ApplyMeshData(MoveTemp(MovedMeshData));
				}
			}
		});
	});
}

void AChunkBase::CreateProcMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FVector2D>& UV1, const TArray<FVector2D>& UV2, const TArray<FVector2D>& UV3, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision, FProcMeshSection& OutSection)
{	
	// Reset this section (in case it already existed)
	OutSection.Reset();

	// Copy data to vertex buffer
	const int32 NumVerts = Vertices.Num();
	OutSection.ProcVertexBuffer.Reset();
	OutSection.ProcVertexBuffer.AddUninitialized(NumVerts);
	for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
	{
		FProcMeshVertex& Vertex = OutSection.ProcVertexBuffer[VertIdx];

		Vertex.Position = Vertices[VertIdx];
		Vertex.Normal = (Normals.Num() == NumVerts) ? Normals[VertIdx] : FVector(0.f, 0.f, 1.f);
		Vertex.UV0 = (UV0.Num() == NumVerts) ? UV0[VertIdx] : FVector2D(0.f, 0.f);
		Vertex.UV1 = (UV1.Num() == NumVerts) ? UV1[VertIdx] : FVector2D(0.f, 0.f);
		Vertex.UV2 = (UV2.Num() == NumVerts) ? UV2[VertIdx] : FVector2D(0.f, 0.f);
		Vertex.UV3 = (UV3.Num() == NumVerts) ? UV3[VertIdx] : FVector2D(0.f, 0.f);
		Vertex.Color = (VertexColors.Num() == NumVerts) ? VertexColors[VertIdx] : FColor(255, 255, 255);
		Vertex.Tangent = (Tangents.Num() == NumVerts) ? Tangents[VertIdx] : FProcMeshTangent();

		// Update bounding box
		OutSection.SectionLocalBox += Vertex.Position;
	}

	// Get triangle indices, clamping to vertex range
	const int32 MaxIndex = NumVerts - 1;
	const auto GetTriIndices = [&Triangles, MaxIndex](int32 Idx)
	{
		return TTuple<int32, int32, int32>(FMath::Min(Triangles[Idx    ], MaxIndex),
										   FMath::Min(Triangles[Idx + 1], MaxIndex),
			                               FMath::Min(Triangles[Idx + 2], MaxIndex));
	};

	const int32 NumTriIndices = (Triangles.Num() / 3) * 3; // Ensure number of triangle indices is multiple of three

	// Detect degenerate triangles, i.e. non-unique vertex indices within the same triangle
	int32 NumDegenerateTriangles = 0;
	for (int32 IndexIdx = 0; IndexIdx < NumTriIndices; IndexIdx += 3)
	{
		int32 a, b, c;
		Tie(a, b, c) = GetTriIndices(IndexIdx);
		NumDegenerateTriangles += a == b || a == c || b == c; //-V614
	}
	if (NumDegenerateTriangles > 0)
	{
		UT_LOG("Detected %d degenerate triangle%s with non-unique vertex indices for created mesh section in '%s'; degenerate triangles will be dropped.",
			   NumDegenerateTriangles, NumDegenerateTriangles > 1 ? TEXT("s") : TEXT(""), TEXT("static"));
	}

	// Copy index buffer for non-degenerate triangles
	OutSection.ProcIndexBuffer.Reset();
	OutSection.ProcIndexBuffer.AddUninitialized(NumTriIndices - NumDegenerateTriangles * 3);
	int32 CopyIndexIdx = 0;
	for (int32 IndexIdx = 0; IndexIdx < NumTriIndices; IndexIdx += 3)
	{
		int32 a, b, c;
		Tie(a, b, c) = GetTriIndices(IndexIdx);

		if (a != b && a != c && b != c) //-V614
		{
			OutSection.ProcIndexBuffer[CopyIndexIdx++] = a;
			OutSection.ProcIndexBuffer[CopyIndexIdx++] = b;
			OutSection.ProcIndexBuffer[CopyIndexIdx++] = c;
		}
		else
		{
			--NumDegenerateTriangles;
		}
	}
	check(NumDegenerateTriangles == 0);
	check(CopyIndexIdx == OutSection.ProcIndexBuffer.Num());

	OutSection.bEnableCollision = bCreateCollision;

}

double AChunkBase::GetUnitVoxelLength() const
{
	return GetChunkManager()->GetUnitVoxelLength();
}

int AChunkBase::GetSeaLevel() const
{
	int SeaLevel = GetChunkManager()->GetSeaLevel();
	if (SeaLevel == 0)
	{
		SeaLevel = ChunkSizeVec.Z / 2;
	}
	return SeaLevel;
}

float AChunkBase::GetFastNoise(const float InX, const float InY) const
{
	return GetChunkManager()->GetFastNoise(InX, InY);
}

float AChunkBase::GetFastNoise(const float InX, const float InY, const float InZ) const
{
	return GetChunkManager()->GetFastNoise(InX, InY, InZ);
}

float AChunkBase::GetFastNoiseByIdx(const float InX, const float InY, const float InZ) const
{
	float Noise = GetFastNoise(InX, InY, InZ); // -1~1

	Noise = std::abs(Noise); // 0 ~ 1

	return Noise;
}

float AChunkBase::GetFastNoiseByIdx(const FIdxVector& InVoxelIdx) const
{
	return GetFastNoiseByIdx(InVoxelIdx.X, InVoxelIdx.Y, InVoxelIdx.Z);
}

int AChunkBase::Get2DHeight(const float InX, const float InY) const
{
	const float Noise    = (GetFastNoise(InX, InY) + 1.0f) / 2.0; // -1~1 -> 0~2 -> 0~1
	const float NoiseToZ = Noise * ChunkSizeVec.Z;                       // Z축에 노이즈 적용
	return FMath::Clamp(FMath::RoundToInt(NoiseToZ), 0, ChunkSizeVec.Z);
}

float AChunkBase::Get2DHeightF(const float InX, const float InY) const 
{
	const float Noise    = (GetFastNoise(InX, InY) + 1.0f) / 2.0; // -1~1 -> 0~2 -> 0~1
	const float NoiseToZ = Noise * ChunkSizeVec.Z;                       // Z축에 노이즈 적용
	return FMath::Clamp(NoiseToZ, 0.0f, static_cast<float>(ChunkSizeVec.Z));
}

int AChunkBase::GetTextureIndex(const EBlockType InBlockType, const FVector& InNormal)
{
	switch (InBlockType)
	{
		case EBlockType::Grass:
			if (InNormal == FVector::UpVector)
			{
				return 0;
			}
			return 1;

		case EBlockType::Dirt:
			return 2;
		case EBlockType::Stone:
			return 3;
		case EBlockType::Air:
			return 4;
		default:
			return 255;
	}
}

FColor AChunkBase::GetTextureIndexColor(const EBlockType InBlockType, const FVector& InNormal)
{
	// FColor(0, 0, 0, GetTextureIndex(InMask.BlockType, Normal))
	return FColor(GetTextureIndex(InBlockType, InNormal), 0, 0, 0);	
}

bool AChunkBase::IsOutOfChunkBoundary(const FIdxVector& InVoxelIdx) const
{
	// 청크 각 변의 크기
	const int Size_X = ChunkVoxels.front().front().size();
	const int Size_Y = ChunkVoxels.front().size();
	const int Size_Z = ChunkVoxels.size();

	if (InVoxelIdx.X >= Size_X || InVoxelIdx.Y >= Size_Y || InVoxelIdx.Z >= Size_Z || InVoxelIdx.X < 0 || InVoxelIdx.Y < 0 || InVoxelIdx.Z < 0)
		return true;

	return false;
}

float AChunkBase::GetVoxelValue(const FIdxVector& InVoxelIdx) const
{
	if (IsOutOfChunkBoundary(InVoxelIdx))
		return -1.0;

	return ChunkVoxels[InVoxelIdx.Z][InVoxelIdx.Y][InVoxelIdx.X];
}

EBlockType AChunkBase::GetBlockType(const FIdxVector& InVoxelIdx) const
{
	return GetBlockType(GetVoxelValue(InVoxelIdx));
}

EBlockType AChunkBase::GetBlockType(const float InNoise) const
{
	auto [BlockType, Weight] = GetBlockTypeAndWeight(InNoise);
	return BlockType;
}

std::tuple<EBlockType, float> AChunkBase::GetBlockTypeAndWeight(const float InNoise) const
{
	// InNoise : -1~1
	if (InNoise <= 0.0f)
	{
		return {EBlockType::Air, 0.0f};
	}

	// ~0.5 정도에서 값이 멈춤.
	const float Weight      = InNoise * 3.0f;                                    // 0.0 ~ 3.0
	const int TypeNumber    = FMath::CeilToInt(Weight);                          // 1 ~ 3
	const float DeltaWeight = Weight - FMath::FloorToInt(Weight);                // 0.0~1.0
	float VertexWeight      = (1.0 - SurfaceLevel) * DeltaWeight + SurfaceLevel; // SurfaceLevel 기점으로 deltaweight이 차지하는 비중 퍼센트 

	EBlockType BlockType;

	switch (TypeNumber)
	{
		case 0:
			BlockType = EBlockType::Air;
			break;
		case 1:
			BlockType = EBlockType::Grass;
			break;
		case 2:
			BlockType = EBlockType::Dirt;
			break;
		case 3:
			BlockType = EBlockType::Stone;
			break;
		default:
			BlockType = EBlockType::Stone;
	}

	return {BlockType, VertexWeight};
}

float AChunkBase::GetNoiseFromBlockType(const EBlockType InBlockType)
{
	float ResNoise = 0.0f;
	switch (InBlockType)
	{
		case EBlockType::Air:
			ResNoise = 0.0f;
		break;
		case EBlockType::Grass:
			ResNoise = 1.0f;
			break;
		case EBlockType::Dirt:
			ResNoise = 2.0f;
			break;
		case EBlockType::Stone:
			ResNoise = 3.0f;
			break;
		default:
			ResNoise = 0.0f;
			break;
	}
	
	return ResNoise / 3.0f;
}

bool AChunkBase::IsOutOfChunkBoundary_Static(const FIdxVector& InVoxelIdx, const FVoxel3DArray& InChunkVoxels)
{
	// 청크 각 변의 크기
	const int Size_X = InChunkVoxels.front().front().size();
	const int Size_Y = InChunkVoxels.front().size();
	const int Size_Z = InChunkVoxels.size();

	if (InVoxelIdx.X >= Size_X || InVoxelIdx.Y >= Size_Y || InVoxelIdx.Z >= Size_Z || InVoxelIdx.X < 0 || InVoxelIdx.Y < 0 || InVoxelIdx.Z < 0)
		return true;

	return false;
}

float AChunkBase::GetVoxelValue_Static(const FIdxVector& InVoxelIdx, const FVoxel3DArray& InChunkVoxels)
{
	if (IsOutOfChunkBoundary_Static(InVoxelIdx, InChunkVoxels))
		return -1.0;

	return InChunkVoxels[InVoxelIdx.Z][InVoxelIdx.Y][InVoxelIdx.X];
}

EBlockType AChunkBase::GetBlockType_Static(const FIdxVector& InVoxelIdx, const FVoxel3DArray& InChunkVoxels, const float InSurfaceLevel)
{
	return GetBlockType_Static(GetVoxelValue_Static(InVoxelIdx, InChunkVoxels), InSurfaceLevel);
}

EBlockType AChunkBase::GetBlockType_Static(const float InNoise, const float InSurfaceLevel)
{
	auto [BlockType, Weight] = GetBlockTypeAndWeight_Static(InNoise, InSurfaceLevel);
	return BlockType;
}

std::tuple<EBlockType, float> AChunkBase::GetBlockTypeAndWeight_Static(const float InNoise, const float InSurfaceLevel)
{
	// InNoise : -1~1
	if (InNoise <= 0.0f)
	{
		return {EBlockType::Air, 0.0f};
	}

	// ~0.5 정도에서 값이 멈춤.
	const float Weight      = InNoise * 3.0f;                                        // 0.0 ~ 3.0
	const int TypeNumber    = FMath::CeilToInt(Weight);                              // 1 ~ 3
	const float DeltaWeight = Weight - FMath::FloorToInt(Weight);                    // 0.0~1.0
	float VertexWeight      = (1.0 - InSurfaceLevel) * DeltaWeight + InSurfaceLevel; // SurfaceLevel 기점으로 deltaweight이 차지하는 비중 퍼센트 

	EBlockType BlockType;

	switch (TypeNumber)
	{
		case 0:
			BlockType = EBlockType::Air;
			break;
		case 1:
			BlockType = EBlockType::Grass;
			break;
		case 2:
			BlockType = EBlockType::Dirt;
			break;
		case 3:
			BlockType = EBlockType::Stone;
			break;
		default:
			BlockType = EBlockType::Stone;
	}

	return {BlockType, VertexWeight};
}

void AChunkBase::ApplyMeshData(FChunkMeshData&& InChunkMeshData)
{
	MeshData = MoveTemp(InChunkMeshData);
	ApplyMesh();
}

void AChunkBase::ApplyMesh()
{
	// 메시 idx 갱신
	ChunkMeshIdx = ChunkPositionIdx;

	ProceduralMesh->bUseAsyncCooking      = GetChunkManager()->IsUsedAsyncPhysicsCooking();
	AsyncProceduralMesh->bUseAsyncCooking = GetChunkManager()->IsUsedAsyncPhysicsCooking();
	DynamicMesh->bUseAsyncCooking         = GetChunkManager()->IsUsedAsyncPhysicsCooking();

	if (GetChunkManager()->IsUsedAsyncCalculateMeshData())
	{
		switch (GetChunkManager()->GetChunkGenType())
		{
		case EChunkMeshGenerateType::UsedCreateMeshSection:
		{
			ProceduralMesh->CreateMeshSection(0
			                                , MeshData.Vertices
			                                , MeshData.Triangles
			                                , MeshData.Normals
			                                , MeshData.UV0
			                                , MeshData.Colors
			                                , TArray<FProcMeshTangent>()
			                                , true);
			break;
		}
		case EChunkMeshGenerateType::UsedSetProcMove:
		{
			ProceduralMesh->SetProcMeshSection(0, FProcMeshSection());
			FProcMeshSection* ProcSection = ProceduralMesh->GetProcMeshSection(0);
			*ProcSection = MoveTemp(MeshData.NewMeshSection);

			ProceduralMesh->SetProcMeshSection(1, FProcMeshSection());
			break;
		}
		case EChunkMeshGenerateType::UsedAsyncProcMesh:
		{
			AsyncProceduralMesh->SetAsyncProcMeshSection(0, MoveTemp(MeshData.NewMeshSection));
			break;
		}
		case EChunkMeshGenerateType::UsedDynamic:
		{
			DynamicMesh->SetMesh(MoveTemp(MeshData.DynamicMesh3));
			break;
		}
		default:
		{
		};
		}
	}
	else
	{
		ProceduralMesh->CreateMeshSection(0
		                                , MeshData.Vertices
		                                , MeshData.Triangles
		                                , MeshData.Normals
		                                , MeshData.UV0
		                                , MeshData.Colors
		                                , TArray<FProcMeshTangent>()
		                                , true);
	}

	RootComponent->SetVisibility(true, true);
}

void AChunkBase::ClearMesh()
{
	MeshData.Clear();
}

void AChunkBase::ResetMeshQueryID()
{
	MeshQueryID = INVALID_QUERY_ID;
}

void AChunkBase::SetMeshQueryID()
{
	MeshQueryID = GetUniqueQueryID();
}
