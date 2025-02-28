// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "kkumigi/KuObject.h"
#include "kkumigi/KuTypes.h"
#include "kkumigi/Voxel/Utils/Enums.h"
#include "ChunkManager.generated.h"

class AChunkWorld;
class FastNoiseLite;
class AChunkBase;

/**
 * 청크 데이터 관리 매니저
 */
UCLASS(Blueprintable, BlueprintType)
class KKUMIGI_API UChunkManager : public UKuObject
{
	GENERATED_BODY()

public:
	UChunkManager();

	virtual void Init() override;
	virtual void BeginDestroy() override;

	void PrevLevelBeginPlay();
	void PostLevelBeginPlay();


	float GetFastNoise(const float InX, const float InY) const;
	float GetFastNoise(const float InX, const float InY, const float InZ) const;

	void SetChunkWorld(AChunkWorld* InChunkWorld);
	AChunkWorld* GetChunkWorld() const;

	FIdxVector GetChunkVecSize() const;

	/** 단위 복셀 길이를 가져옵니다.*/
	double GetUnitVoxelLength() const { return UnitVoxelLength; }

	/** 해수면 기준 높이를 가져옵니다. */
	int GetSeaLevel() const { return SeaLevel; }

	int GetFrontRenderDist() const { return FrontRenderDist; }
	int GetBackRenderDist() const { return BackRenderDist; }
	int GetWidthRenderDist() const { return WidthRenderDist; }

	bool IsUseSmoothingNormal() const { return bUseSmoothingNormal; }
	bool IsUsedAsyncCalculateMeshData() const { return bUsedAsyncCalculateMeshData; }
	bool IsUsedDynamicMesh() const { return bUsedDynamicMesh; }
	bool IsUsedProceduralMesh() const { return bUsedDynamicMesh == false; }


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<AChunkWorld> ChunkWorldClass;

protected:
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AChunkWorld> ChunkWorld;

	TPimplPtr<FastNoiseLite> FastNoise;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int ChunkNoiseSeed;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float NoiseFrequency;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	uint32 ChunkSize_X;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	uint32 ChunkSize_Y;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	uint32 ChunkSize_Z;

	/** 복셀의 단위 길이입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double UnitVoxelLength;

	/** 해수면 기준 높이. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SeaLevel;

	/** 렌더링할 청크 거리. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FrontRenderDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BackRenderDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WidthRenderDist;

	/** vertex 노말을 주변 삼각형의 노말에 맞춰 스무딩할지 지정합니다. */
	UPROPERTY(EditDefaultsOnly)
	bool bUseSmoothingNormal;

	/** 비동기 메시 데이터 계산 사용 유무 */
	UPROPERTY(EditDefaultsOnly)
	bool bUsedAsyncCalculateMeshData;

	/** 다이나믹 메시 사용 유무. false면 ProceduralMesh 사용 */
	UPROPERTY(EditDefaultsOnly)
	bool bUsedDynamicMesh;
};
