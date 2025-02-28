// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "GameFramework/Actor.h"
#include "kkumigi/KuActor.h"
#include "kkumigi/KuTypes.h"

#include "kkumigi/Voxel/Utils/Enums.h"

#include <deque>
#include <queue>

#include "ChunkWorld.generated.h"

class AChunkBase;

UCLASS()
class AChunkWorld final : public AKuActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChunkWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	void GenerateWorld();

	/**
	 * 로드할 청크를 대기열에 넣습니다.
	 * 대기열의 청크는 매 틱마다 나눠서 로드됩니다.
	 */
	void EnqueueLoadChunk(const FIntVector& InChunkPosIdx);

	/**
	 * 매 틱마다 호출되어 대기열의 청크를 로드합니다.
	 */
	void ExecuteTickLoadChunk(const float DeltaTime);

	/**
	 * 청크를 로드합니다.
	 * 청크풀에 청크가 있다면 재활용하고, 아니라면 새로 스폰합니다.
	 */
	void LoadChunk(const FIntVector& InChunkPosIdx);
	

	/**
	 * 대상 청크를 비활성화 후, ChunkPool에 대기시킵니다.
	 * 저장해야한다면 저장도 합니다.
	 */
	void UnloadChunk(TWeakObjectPtr<AChunkBase> InChunk);

public:
	/**
	 * 플레이어 위치를 업데이트 합니다.
	 * 청크를 로드, 언로드할 때 기준이 됩니다. 
	 */
	void UpdatePlayerLocation(const FVector& InPlayerLocation);

	/**
	 * 월드 위치를 기반으로 청크 내 복셀을 수정합니다.
	 * 1개 이상의 청크를 수정합니다.
	 * 
	 * @param InVoxelPosList 수정할 복셀의 월드 위치 리스트 
	 * @param InBlockType 지정할 타입
	 */
	void ModifyChunks(const TArray<FVector>& InVoxelPosList, const EBlockType InBlockType);

	/** 청크의 월드 위치를 반환합니다. */
	FVector GetChunkWorldPosFromIdx(const FIntVector& InChunkPosIdx) const;

	/** 청크 상대 위치 인덱스를 반환합니다. */
	FIntVector GetChunkIdxFromWorldPos(const FVector& InWorldPos) const;

	/**
	 * 입력된 월드 위치가 있는 청크의 Idx와 블록 Idx를 구합니다.
	 * 
	 * @param InPosition 월드 위치.
	 * @return 청크 Idx, 블록 Idx
	 */
	std::tuple<FIntVector, FIdxVector> GetChunkAndBlockIdxFromWorldPos(const FVector& InPosition) const;

	double GetUnitVoxelLength() const;

public:
	UPROPERTY(EditAnywhere, Category="Chunk World")
	TSubclassOf<AChunkBase> ChunkClassType;

	/** 청크의 사이즈입니다. 별칭을 사용으로 UPROPERTY 매크로 이용불가. */
	FIdxVector ChunkSizeVec;

	UPROPERTY(EditAnywhere, Category="Chunk World")
	TObjectPtr<UMaterialInterface> Material;

protected:
	/** 미사용 중인 청크를 보관하는 풀 */
	hash_queue<FIntVector, TWeakObjectPtr<AChunkBase>> ChunkPoolQueue;

	UPROPERTY(VisibleAnywhere, Category="Chunk World")
	TMap<FIntVector, TWeakObjectPtr<AChunkBase>> ChunkList;

	struct FWaitChunk
	{
		float DistSquared2D; // 플레이어까지의 거리. 2차원 평면에서 거리의 제곱.
		FIntVector ChunkIdx; // 생성 대기중인 청크의 인덱스

		FWaitChunk(const FIntVector& InChunkIdx, const FIntVector& InPlayerChunkIdx)
		{
			ChunkIdx      = InChunkIdx;
			DistSquared2D = FVector3f(InPlayerChunkIdx - InChunkIdx).SizeSquared2D();
		}

		FORCEINLINE auto operator<=>(const FWaitChunk& Other) const
		{
			return DistSquared2D <=> Other.DistSquared2D;
		}
	};

	/**
	 * 로드 대기 중인 청크.
	 * 플레이어와 가장 가까운 순으로 정렬되어 보관합니다.
	 */
	std::priority_queue<FWaitChunk, std::vector<FWaitChunk>, std::greater<>> WaitLoadChunkQ;

	/**
	 * 플레이어가 위치한 청크 Index를 기록합니다.
	 * 플레이어가 다른 청크로 이동했는지 확인할 때 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, Category="Chunk World")
	FIntVector CachedPlayerChunkIdx;

	/**
	 * 플레이어가 위치한 블록 Idx를 기록합니다.
	 * 복셀과 겹치는지 확인할 때 사용합니다.
	 */
	FIdxVector CachedPlayerBlockIdx;
};
