// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "kkumigi/KuActor.h"
#include "kkumigi/KuTypes.h"

#include "kkumigi/Voxel/Utils/ChunkMeshData.h"
#include "kkumigi/Voxel/Utils/Enums.h"

#include "ChunkBase.generated.h"


class UAsyncProceduralMeshComponent;
struct FProcMeshSection;
class UDynamicMeshComponent;
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS(Abstract)
class KKUMIGI_API AChunkBase : public AKuActor
{
	GENERATED_BODY()
	friend struct FAsyncChunkMeshData;

public:
	// Sets default values for this actor's properties
	AChunkBase();

	/** 최초 청크 생성 시, 청크를 초기화합니다. */
	void InitializeChunk(const FIntVector& InChunkPosIdx, const FIdxVector& InChunkSize, UMaterialInterface* InMaterial);

	/** 이미 생성된 청크를 다시 사용할 때 사용합니다. */
	void ReplaceChunk(const FIntVector& InChunkPosIdx, const FIdxVector& InChunkSize, UMaterialInterface* InMaterial);

	/** 청크를 언로드 및 대기상태로 전홥니다. */
	void WaitChunk();

protected:
	virtual void ReserveChunkVoxels();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void ModifyChunk(const TArray<FIdxVector>& InVoxelList, const EBlockType InBlock);

	void ModifyVoxelData(const FIdxVector& InVoxelIdx, const EBlockType InBlock);
	void ModifyVoxelData(const FIdxVector& InVoxelIdx, const float InNoise);

	void SaveChunkData();
	void LoadChunkData();

protected:
	/**
	 * 청크의 데이터를 생성합니다.
	 * 각 복셀의 값은 해당 복셀의 월드 위치에 기반합니다.
	 * 
	 * @param InPosition 청크 상대 위치(index)
	 */
	void InitializeChunkData(const FVector& InPosition);

	/**
	 * MeshData를 기반으로 청크의 메시를 생성합니다.
	 */
	void CreateChunkMesh();

	/**
	 * Pure Virtual
	 * 메시를 구성할 MeshData를 계산합니다.
	 */
	virtual void CalculateMeshData() PURE_VIRTUAL(AChunkBase::CalculateMeshData);

	/**
	 * 비동기 MeshData 계산 후, 메시를 생성합니다.
	 * 자식 클래스의 static 함수를 사용하기 위해 pure virtual 선언되었습니다. 
	 */
	virtual void AsyncGenerateMesh() PURE_VIRTUAL(AChunkBase::AsyncGenerateMesh);

	/**
	 * 비동기 MeshData 계산 후, 메시를 생성합니다.
	 * 자식클래스의 static 함수를 사용합니다.
	 * 
	 * @param InCalculateMeshDataFunc AsyncTask에서 사용할 MeshData 계산함수. 자식 클래스의 static 함수를 사용합니다. 
	 */
	void ExecuteAsyncGenerateMesh(TFunction<void(const FAsyncChunkMeshData& InAsyncData, FChunkMeshData& OutMeshData)>&& InCalculateMeshDataFunc);

	static void CreateProcMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FVector2D>& UV1, const TArray<FVector2D>& UV2, const TArray<FVector2D>& UV3, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision, FProcMeshSection& OutSection);

	/** 단위 복셀의 길이를 가져옵니다. */
	double GetUnitVoxelLength() const;

	int GetSeaLevel() const;

	float GetFastNoise(const float InX, const float InY) const;
	float GetFastNoise(const float InX, const float InY, const float InZ) const;

	/**
	 * 해당 좌표의 노이즈 값을 출력합니다.
	 * 
	 * @return 0~1. 0 근처에 밀집된 값을 출력합니다
	 */
	float GetFastNoiseByIdx(const float InX, const float InY, const float InZ) const;
	float GetFastNoiseByIdx(const FIdxVector& InVoxelIdx) const;

	/**
	 * 해당 2차원 좌표의 높이 노이즈를 반환합니다.
	 * @return 0...ChunkSize.Z 사이의 노이즈 출력.
	 */
	int Get2DHeight(const float InX, const float InY) const;

	float Get2DHeightF(const float InX, const float InY) const;

private:
	static int GetTextureIndex(const EBlockType InBlockType, const FVector& InNormal);

protected:
	static FColor GetTextureIndexColor(const EBlockType InBlockType, const FVector& InNormal);


	/**
	 * 해당 위치가 청크 외부인지 확인합니다.
	 * 
	 * @param InVoxelIdx 확인할 복셀 위치 
	 * @return 청크 외부라면 true.
	 */
	bool IsOutOfChunkBoundary(const FIdxVector& InVoxelIdx) const;
	float GetVoxelValue(const FIdxVector& InVoxelIdx) const;
	EBlockType GetBlockType(const FIdxVector& InVoxelIdx) const;
	EBlockType GetBlockType(const float InNoise) const;
	std::tuple<EBlockType, float> GetBlockTypeAndWeight(const float InNoise) const;

	static float GetNoiseFromBlockType(const EBlockType InBlockType);

public:
	FIntVector GetChunkPositionIdx() const { return ChunkPositionIdx; };

	// ~=====================================
	// 비동기용 static 함수
	static bool IsOutOfChunkBoundary_Static(const FIdxVector& InVoxelIdx, const FVoxel3DArray& InChunkVoxels);
	static float GetVoxelValue_Static(const FIdxVector& InVoxelIdx, const FVoxel3DArray& InChunkVoxels);
	static EBlockType GetBlockType_Static(const FIdxVector& InVoxelIdx, const FVoxel3DArray& InChunkVoxels, const float InSurfaceLevel);
	static EBlockType GetBlockType_Static(const float InNoise, const float InSurfaceLevel);
	static std::tuple<EBlockType, float> GetBlockTypeAndWeight_Static(const float InNoise, const float InSurfaceLevel);

	void ApplyMeshData(FChunkMeshData&& InChunkMeshData);

protected:
	void ApplyMesh();
	void ClearMesh();


	/** 청크의 상대 위치를 나타내는 인덱스 */
	UPROPERTY(VisibleAnywhere)
	FIntVector ChunkPositionIdx;

	/**
	 * 청크에 실제로 적용된 메시의 인덱스.
	 * 메시를 재계산 없이 사용할 수 있는지 확인할 때 사용합니다. 
	 */
	UPROPERTY(VisibleAnywhere)
	FIntVector ChunkMeshIdx;

	/** 청크의 크기. 청크의 각 변을 구성하는 복셀의 개수입니다. */
	FIdxVector ChunkSizeVec;

	UPROPERTY(EditDefaultsOnly, Category="Marching Cubes")
	float SurfaceLevel;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProceduralMeshComponent> ProceduralMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAsyncProceduralMeshComponent> AsyncProceduralMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDynamicMeshComponent> DynamicMesh;

	FChunkMeshData MeshData;

	FVoxel3DArray ChunkVoxels;

	/**
	 * 청크 데이터가 수정되었다면 true.
	 * 최초 생성 혹은 저장 후 수정되지 않았다면 false.
	 */
	bool bModifiedData;


	/**
	 * 비동기 메시 생성 요청 ID입니다.
	 * 비동기 요청 완료 후, 요청이 여전히 유효한지 판단할 때 사용합니다.
	 */
	TQueryID MeshQueryID;

	void ResetMeshQueryID();
	void SetMeshQueryID();
	FORCEINLINE static TQueryID GetUniqueQueryID()
	{
		static TQueryID LastQueryUniqueID = INVALID_QUERY_ID;
		return ++LastQueryUniqueID;
	}

public:
	TQueryID GetMeshQueryID() const { return MeshQueryID; };
};
