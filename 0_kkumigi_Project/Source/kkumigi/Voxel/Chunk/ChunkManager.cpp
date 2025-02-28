// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkManager.h"

#include "kkumigi/KuTypes.h"
#include "kkumigi/KuGameInstance.h"
#include "kkumigi/KuGameStateBase.h"
#include "kkumigi/KuPlayData.h"
#include "kkumigi/Voxel/Utils/FastNoiseLite.h"
#include "kkumigi/Voxel/World/ChunkWorld.h"

#include "UtlLog.h"

// Sets default values
UChunkManager::UChunkManager()
{
	ChunkNoiseSeed = 1337;
	NoiseFrequency = 0.03f;

	ChunkSize_X  = 32;
	ChunkSize_Y  = 32;
	ChunkSize_Z  = 32;

	UnitVoxelLength = 25.0f;
	SeaLevel = 0;

	FrontRenderDist = 7;
	BackRenderDist = 2;
	WidthRenderDist = 7;

	bUseSmoothingNormal = true;
	bUsedAsyncCalculateMeshData = true;
	bUsedDynamicMesh = false;
}

void UChunkManager::Init()
{
	Super::Init();

	FastNoise = MakePimpl<FastNoiseLite>();

	FastNoise->SetSeed(ChunkNoiseSeed);
	FastNoise->SetFrequency(NoiseFrequency);
	FastNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	FastNoise->SetFractalType(FastNoiseLite::FractalType_FBm);


	FKuWorldDelegates::OnPrevLevelBeginPlay.AddUObject(this, &UChunkManager::PrevLevelBeginPlay);
	FKuWorldDelegates::OnPostLevelBeginPlay.AddUObject(this, &UChunkManager::PostLevelBeginPlay);
}

void UChunkManager::BeginDestroy()
{
	FKuWorldDelegates::OnPrevLevelBeginPlay.RemoveAll(this);
	FKuWorldDelegates::OnPostLevelBeginPlay.RemoveAll(this);

	Super::BeginDestroy();
}

void UChunkManager::PrevLevelBeginPlay()
{
	if (UWorld* World = GetWorld())
	{
		ChunkWorld = World->SpawnActor<AChunkWorld>(ChunkWorldClass);
	}
}

void UChunkManager::PostLevelBeginPlay()
{
}

float UChunkManager::GetFastNoise(const float InX, const float InY) const
{
	return FastNoise->GetNoise(InX, InY);
}

float UChunkManager::GetFastNoise(const float InX, const float InY, const float InZ) const
{
	return FastNoise->GetNoise(InX, InY, InZ);
}

void UChunkManager::SetChunkWorld(AChunkWorld* InChunkWorld)
{
	if (ChunkWorld.IsValid() == false)
	{
		ChunkWorld = InChunkWorld;
	}
	else
	{
		UT_LOG("ChunkWorld is already exists")
	}
}

AChunkWorld* UChunkManager::GetChunkWorld() const
{
	return ChunkWorld.Get();
}

FIdxVector UChunkManager::GetChunkVecSize() const
{
	return FIdxVector(ChunkSize_X, ChunkSize_Y, ChunkSize_Z);
}
