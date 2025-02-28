// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkWorld.h"

#include "kkumigi/Voxel/Chunk/ChunkBase.h"
#include "Kismet/GameplayStatics.h"
#include "kkumigi/KuGameInstance.h"
#include "kkumigi/KuPlayData.h"
#include "kkumigi/Voxel/Chunk/ChunkManager.h"
#include "kkumigi/Voxel/Chunk/NaiveChunk.h"

#include "UTLStatics.h"

#include <deque>
#include <queue>

// Sets default values
AChunkWorld::AChunkWorld()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChunkClassType   = ANaiveChunk::StaticClass();
	ChunkSizeVec     = FIdxVector(0);

	CachedPlayerChunkIdx = FIntVector(0);
}

// Called when the game starts or when spawned
void AChunkWorld::BeginPlay()
{
	Super::BeginPlay();

	ChunkSizeVec = GetChunkManager()->GetChunkVecSize();

	GenerateWorld();

	UT_LOG("Chunk Created : %d", ChunkList.Num())
}

void AChunkWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ExecuteTickLoadChunk(DeltaTime);
}

void AChunkWorld::GenerateWorld()
{
	// 완충용으로 더 넓게 생성
	const int FrontDist = GetChunkManager()->GetFrontRenderDist() + 1;
	const int BackDist  = GetChunkManager()->GetBackRenderDist() + 1;
	const int WidthDist = GetChunkManager()->GetWidthRenderDist() + 1;

	const int FrontIdx = CachedPlayerChunkIdx.X + FrontDist;
	const int BackIdx  = CachedPlayerChunkIdx.X - BackDist;
	const int LeftIdx  = CachedPlayerChunkIdx.Y - WidthDist;
	const int RightIdx = CachedPlayerChunkIdx.Y + WidthDist;

	for (int IdxX = BackIdx; IdxX <= FrontIdx; ++IdxX)
	{
		for (int IdxY = LeftIdx; IdxY <= RightIdx; ++IdxY)
		{
			LoadChunk(FIntVector(IdxX, IdxY, 0));
		}
	}
}

void AChunkWorld::EnqueueLoadChunk(const FIntVector& InChunkPosIdx)
{
	WaitLoadChunkQ.emplace(InChunkPosIdx, CachedPlayerChunkIdx);
}

void AChunkWorld::ExecuteTickLoadChunk(const float DeltaTime)
{
	// 40 프레임 이하에서는 프레임 당 2회 반복
	constexpr static float OverTime = 1.0f / 40.0f;
	const int Count = DeltaTime > OverTime ? 1 : 1;
	for (int i = 0; i < Count; ++i)
	{
		if (WaitLoadChunkQ.empty())
		{
			return;
		}
		const FIntVector TargetChunkIdx = WaitLoadChunkQ.top().ChunkIdx;
		WaitLoadChunkQ.pop();
		LoadChunk(TargetChunkIdx);
	}
}

void AChunkWorld::LoadChunk(const FIntVector& InChunkPosIdx)
{
	if (ChunkList.Contains(InChunkPosIdx))
	{
		UT_LOG("Chunk already exists.");
		return;
	}
	const FVector SpawnPos = GetChunkWorldPosFromIdx(InChunkPosIdx);
	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnPos, FVector::OneVector);

	AChunkBase* ChunkBase = nullptr;

	if (ChunkPoolQueue.empty() == false)
	{
		// 동일 위치에 있던 청크가 있다면 해당 청크 사용.
		if (const TWeakObjectPtr<AChunkBase>* FindPtr = ChunkPoolQueue.find(InChunkPosIdx))
		{
			ChunkBase = FindPtr->Get();
			ChunkPoolQueue.erase(InChunkPosIdx);
		}
		else
		{
			ChunkBase = ChunkPoolQueue.top().Get();
			ChunkPoolQueue.pop();
		}
	}

	if (ChunkBase != nullptr)
	{
		ChunkBase->SetActorLocation(SpawnPos);
		ChunkBase->ReplaceChunk(InChunkPosIdx, ChunkSizeVec, Material);
	}
	else
	{
		ChunkBase = GetWorld()->SpawnActorDeferred<AChunkBase>(ChunkClassType, SpawnTransform, this);
		ChunkBase->InitializeChunk(InChunkPosIdx, ChunkSizeVec, Material);
		UGameplayStatics::FinishSpawningActor(ChunkBase, SpawnTransform);
	}


	ChunkList.Add(InChunkPosIdx, ChunkBase);
}

void AChunkWorld::UnloadChunk(const TWeakObjectPtr<AChunkBase> InChunk)
{
	if (AChunkBase* ChunkBase = InChunk.Get())
	{
		const FIntVector ChunkIdx = ChunkBase->GetChunkPositionIdx();
		ChunkBase->WaitChunk();
		ChunkList.Remove(ChunkIdx);
		ChunkPoolQueue.push(ChunkIdx, InChunk);
	}
}

void AChunkWorld::UpdatePlayerLocation(const FVector& InPlayerLocation)
{
	// 플레이어 위치 갱신
	const FVector PlayerPos = FVector(UtlMath::MakeFVector2D(InPlayerLocation), 0);
	const auto [PlayerChunkIdx, PlayerBlockIdx] = GetChunkAndBlockIdxFromWorldPos(PlayerPos);
	CachedPlayerBlockIdx = PlayerBlockIdx;

	// 플레이어가 다른 청크 위로 이동했는지 확인합니다.
	if (CachedPlayerChunkIdx == PlayerChunkIdx)
	{
		return;
	}

	CachedPlayerChunkIdx = PlayerChunkIdx;

	const int FrontDist = GetChunkManager()->GetFrontRenderDist();
	const int BackDist  = GetChunkManager()->GetBackRenderDist();
	const int WidthDist = GetChunkManager()->GetWidthRenderDist();

	const FIntVector RenderAreaStart = FIntVector(CachedPlayerChunkIdx.X - BackDist, CachedPlayerChunkIdx.Y - WidthDist, 0);
	const FIntVector RenderAreaEnd   = FIntVector(CachedPlayerChunkIdx.X + FrontDist, CachedPlayerChunkIdx.Y + WidthDist, 0);


	// 범위를 벗어난 청크 언로드.
	TArray<TWeakObjectPtr<AChunkBase>> UnloadTargets;
	for (const auto& [ChunkIdx, ChunkBaseWeak] : ChunkList)
	{
		if (ChunkIdx.X < RenderAreaStart.X || ChunkIdx.Y < RenderAreaStart.Y || ChunkIdx.X > RenderAreaEnd.X || ChunkIdx.Y > RenderAreaEnd.Y)
		{
			UnloadTargets.Add(ChunkBaseWeak);
		}
	}
	for (const TWeakObjectPtr<AChunkBase>& ChunkBaseWeak : UnloadTargets)
	{
		UnloadChunk(ChunkBaseWeak);
	}

	// 대기열 재작성
	// 범위 내부에 없다면, 로드 대기열에 추가
	WaitLoadChunkQ = {};
	
	for (int IdxX = RenderAreaStart.X; IdxX <= RenderAreaEnd.X; ++IdxX)
	{
		for (int IdxY = RenderAreaStart.Y; IdxY <= RenderAreaEnd.Y; ++IdxY)
		{
			const FIntVector ChunkIdx           = FIntVector(IdxX, IdxY, 0);
			TWeakObjectPtr<AChunkBase>* FindPtr = ChunkList.Find(ChunkIdx);
			if (FindPtr == nullptr || FindPtr->Get() == nullptr)
			{
				EnqueueLoadChunk(ChunkIdx);
			}
		}
	}
}

void AChunkWorld::ModifyChunks(const TArray<FVector>& InVoxelPosList, const EBlockType InBlockType)
{
	// key: 변경 대상 청크, val: 해당 청크에서 변경할 셀
	TMap<TWeakObjectPtr<AChunkBase>, TArray<FIdxVector>> TargetChunkList;
	for (const FVector& TargetVoxel : InVoxelPosList)
	{
		// FIntVector ChunkIdx;
		// FIdxVector BlockIdx;
		const auto [ChunkIdx, BlockIdx] = GetChunkAndBlockIdxFromWorldPos(TargetVoxel);
		if (TWeakObjectPtr<AChunkBase>* TargetChunk = ChunkList.Find(ChunkIdx))
		{
			TargetChunkList.FindOrAdd(*TargetChunk).Add(FIdxVector(BlockIdx));
		}

		// 마칭큐브 전용 옆 청크 마지막
		if (BlockIdx.X == 0)
		{
			if (TWeakObjectPtr<AChunkBase>* TargetChunk = ChunkList.Find(ChunkIdx - FIntVector(1, 0, 0)))
			{
				FIdxVector PrevIdx = BlockIdx;
				PrevIdx.X          = ChunkSizeVec.X;
				TargetChunkList.FindOrAdd(*TargetChunk).Add(PrevIdx);
			}
		}
		if (BlockIdx.Y == 0)
		{
			if (TWeakObjectPtr<AChunkBase>* TargetChunk = ChunkList.Find(ChunkIdx - FIntVector(0, 1, 0)))
			{
				FIdxVector PrevIdx = BlockIdx;
				PrevIdx.Y          = ChunkSizeVec.Y;
				TargetChunkList.FindOrAdd(*TargetChunk).Add(PrevIdx);
			}
		}
		if ((BlockIdx.X == 0) && (BlockIdx.Y == 0))
		{
			if (TWeakObjectPtr<AChunkBase>* TargetChunk = ChunkList.Find(ChunkIdx - FIntVector(1, 1, 0)))
			{
				FIdxVector PrevIdx = BlockIdx;
				PrevIdx.X          = ChunkSizeVec.X;
				PrevIdx.Y          = ChunkSizeVec.Y;
				TargetChunkList.FindOrAdd(*TargetChunk).Add(PrevIdx);
			}
		}
	}

	for (TPair<TWeakObjectPtr<AChunkBase>, TArray<FIdxVector>>& TargetChunkPair : TargetChunkList)
	{
		AChunkBase* Chunkbase = TargetChunkPair.Key.Get();
		TArray<FIdxVector>& IdxList = TargetChunkPair.Value;
		Chunkbase->ModifyChunk(IdxList, InBlockType);
	}
	
}

FVector AChunkWorld::GetChunkWorldPosFromIdx(const FIntVector& InChunkPosIdx) const
{
	// 청크 상대 위치 * 청크 칸 수 * 셀 단위길이
	const FVector SpawnPos = FVector(InChunkPosIdx) * FVector(ChunkSizeVec) * GetUnitVoxelLength();
	return SpawnPos;
}

FIntVector AChunkWorld::GetChunkIdxFromWorldPos(const FVector& InWorldPos) const
{
	// 청크 월드 위치 / 청크 칸 수 / 셀 단위 길이
	// 소수점 이하 내림
	const FIntVector ChunkPosIdx = FIntVector(UtlMath::FloorVector(InWorldPos / FVector(ChunkSizeVec) / GetUnitVoxelLength()));

	return ChunkPosIdx;
}

std::tuple<FIntVector, FIdxVector> AChunkWorld::GetChunkAndBlockIdxFromWorldPos(const FVector& InPosition) const
{
	const double VoxelLength = GetUnitVoxelLength();

	const FIntVector OutChunkIdx = GetChunkIdxFromWorldPos(InPosition);
	const FIdxVector OutBlockIdx = FIdxVector(FIntVector(UtlMath::RoundVector((InPosition - GetChunkWorldPosFromIdx(OutChunkIdx)) / VoxelLength, 1.0)));

	return {OutChunkIdx, OutBlockIdx};
}

double AChunkWorld::GetUnitVoxelLength() const
{
	return GetKuGameInstance()->GetChunkManager()->GetUnitVoxelLength();
}

