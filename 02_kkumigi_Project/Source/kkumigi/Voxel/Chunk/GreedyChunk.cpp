// Fill out your copyright notice in the Description page of Project Settings.


#include "GreedyChunk.h"

#include "kkumigi/Voxel/Utils/FastNoiseLite.h"

#include "UtlLog.h"
#include "UTLStatics.h"

#include <array>

void AGreedyChunk::ReserveChunkVoxels()
{
	ChunkVoxels = std::vector<std::vector<std::vector<float>>>(ChunkSizeVec.Z, std::vector<std::vector<float>>(ChunkSizeVec.Y, std::vector<float>(ChunkSizeVec.X, 0.0f)));
}

void AGreedyChunk::CalculateMeshData()
{
	const std::array<int, 3> AxisSize = {ChunkSizeVec.X, ChunkSizeVec.Y, ChunkSizeVec.Z};
	// Sweep over each axis (X, Y, Z)
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		// 2 Perpendicular axis
		const int Axis1 = (Axis + 1) % 3;
		const int Axis2 = (Axis + 2) % 3;

		const int MainAxisLimit = AxisSize[Axis];
		const int Axis1Limit = AxisSize[Axis1];
		const int Axis2Limit = AxisSize[Axis2];

		FIdxVector DeltaAxis1 = FIdxVector::ZeroValue;
		FIdxVector DeltaAxis2 = FIdxVector::ZeroValue;

		FIdxVector ChunkItr = FIdxVector::ZeroValue;
		FIdxVector AxisMask = FIdxVector::ZeroValue;

		AxisMask[Axis] = 1;

		TArray<FMask> Mask;
		Mask.SetNum(Axis1Limit * Axis2Limit);

		// Check each slice of the chunk
		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
		{
			int N = 0;

			// Compute Mask
			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
			{
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
				{
					const EBlockType CurrentBlock = GetBlockType(ChunkItr);
					const EBlockType CompareBlock = GetBlockType(ChunkItr + AxisMask);

					const bool CurrentBlockOpaque = CurrentBlock != EBlockType::Air;
					const bool CompareBlockOpaque = CompareBlock != EBlockType::Air;

					if (CurrentBlockOpaque == CompareBlockOpaque)
					{
						Mask[N++] = FMask{EBlockType::Null, 0};
					}
					else if (CurrentBlockOpaque)
					{
						Mask[N++] = FMask{CurrentBlock, 1};
					}
					else
					{
						Mask[N++] = FMask{CompareBlock, -1};
					}
				}
			}

			++ChunkItr[Axis];
			N = 0;

			// Generate Mesh From Mask
			for (int j = 0; j < Axis2Limit; ++j)
			{
				for (int i = 0; i < Axis1Limit;)
				{
					if (Mask[N].Normal != 0)
					{
						const auto CurrentMask = Mask[N];
						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int Width;

						for (Width = 1; i + Width < Axis1Limit && CompareMask(Mask[N + Width], CurrentMask); ++Width)
						{
						}

						int Height;
						bool Done = false;

						for (Height = 1; j + Height < Axis2Limit; ++Height)
						{
							for (int k = 0; k < Width; ++k)
							{
								if (CompareMask(Mask[N + k + Height * Axis1Limit], CurrentMask)) continue;

								Done = true;
								break;
							}

							if (Done) break;
						}

						DeltaAxis1[Axis1] = Width;
						DeltaAxis2[Axis2] = Height;

						CreateQuad(
						           CurrentMask
						         , AxisMask
						         , Width
						         , Height
						         , ChunkItr
						         , ChunkItr + DeltaAxis1
						         , ChunkItr + DeltaAxis2
						         , ChunkItr + DeltaAxis1 + DeltaAxis2
						          );

						DeltaAxis1 = FIdxVector::ZeroValue;
						DeltaAxis2 = FIdxVector::ZeroValue;

						for (int l = 0; l < Height; ++l)
						{
							for (int k = 0; k < Width; ++k)
							{
								Mask[N + k + l * Axis1Limit] = FMask{EBlockType::Null, 0};
							}
						}

						i += Width;
						N += Width;
					}
					else
					{
						i++;
						N++;
					}
				}
			}
		}
	}
}

void AGreedyChunk::CreateQuad(const FMask InMask
                            , const FIdxVector& InAxisMask
                            , const int InWidth
                            , const int InHeight
                            , const FIdxVector& InV1
                            , const FIdxVector& InV2
                            , const FIdxVector& InV3
                            , const FIdxVector& InV4)
{
	const FVector Normal = FVector(InAxisMask * InMask.Normal);
	const FColor Color   = GetTextureIndexColor(InMask.BlockType, Normal);

	MeshData.Vertices.Append({
		                         FVector(InV1) * GetUnitVoxelLength()
		                       , FVector(InV2) * GetUnitVoxelLength()
		                       , FVector(InV3) * GetUnitVoxelLength()
		                       , FVector(InV4) * GetUnitVoxelLength()
	                         });

	MeshData.Triangles.Append({
		                          MeshData.VertexCount
		                        , MeshData.VertexCount + 2 + InMask.Normal
		                        , MeshData.VertexCount + 2 - InMask.Normal
		                        , MeshData.VertexCount + 3
		                        , MeshData.VertexCount + 1 - InMask.Normal
		                        , MeshData.VertexCount + 1 + InMask.Normal
	                          });

	MeshData.Normals.Append({
		                        Normal
		                      , Normal
		                      , Normal
		                      , Normal
	                        });

	MeshData.Colors.Append({
		                       Color
		                     , Color
		                     , Color
		                     , Color
	                       });

	if (Normal.X == 1 || Normal.X == -1)
	{
		MeshData.UV0.Append({
			                    FVector2D(InWidth, InHeight)
			                  , FVector2D(0, InHeight)
			                  , FVector2D(InWidth, 0)
			                  , FVector2D(0, 0)
		                    });
	}
	else
	{
		MeshData.UV0.Append({
			                    FVector2D(InHeight, InWidth)
			                  , FVector2D(InHeight, 0)
			                  , FVector2D(0, InWidth)
			                  , FVector2D(0, 0)
		                    });
	}

	MeshData.VertexCount += 4;
}

void AGreedyChunk::AsyncGenerateMesh()
{
	ExecuteAsyncGenerateMesh(AsyncCalculateMeshData);
}

void AGreedyChunk::AsyncCalculateMeshData(const FAsyncChunkMeshData& InAsyncData, FChunkMeshData& OutMeshData)
{
	const std::array<int, 3> AxisSize = {InAsyncData.InChunkSizeVec.X, InAsyncData.InChunkSizeVec.Y, InAsyncData.InChunkSizeVec.Z};
	// Sweep over each axis (X, Y, Z)
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		// 2 Perpendicular axis
		const int Axis1 = (Axis + 1) % 3;
		const int Axis2 = (Axis + 2) % 3;

		const int MainAxisLimit = AxisSize[Axis];
		const int Axis1Limit    = AxisSize[Axis1];
		const int Axis2Limit    = AxisSize[Axis2];

		FIdxVector DeltaAxis1 = FIdxVector::ZeroValue;
		FIdxVector DeltaAxis2 = FIdxVector::ZeroValue;

		FIdxVector ChunkItr = FIdxVector::ZeroValue;
		FIdxVector AxisMask = FIdxVector::ZeroValue;

		AxisMask[Axis] = 1;

		TArray<FMask> Mask;
		Mask.SetNum(Axis1Limit * Axis2Limit);

		// Check each slice of the chunk
		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
		{
			int N = 0;

			// Compute Mask
			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
			{
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
				{
					const EBlockType CurrentBlock = GetBlockType_Static(ChunkItr, InAsyncData.InChunkVoxels, InAsyncData.InSurfaceLevel);
					const EBlockType CompareBlock = GetBlockType_Static(ChunkItr + AxisMask, InAsyncData.InChunkVoxels, InAsyncData.InSurfaceLevel);

					const bool CurrentBlockOpaque = CurrentBlock != EBlockType::Air;
					const bool CompareBlockOpaque = CompareBlock != EBlockType::Air;

					if (CurrentBlockOpaque == CompareBlockOpaque)
					{
						Mask[N++] = FMask{EBlockType::Null, 0};
					}
					else if (CurrentBlockOpaque)
					{
						Mask[N++] = FMask{CurrentBlock, 1};
					}
					else
					{
						Mask[N++] = FMask{CompareBlock, -1};
					}
				}
			}

			++ChunkItr[Axis];
			N = 0;

			// Generate Mesh From Mask
			for (int j = 0; j < Axis2Limit; ++j)
			{
				for (int i = 0; i < Axis1Limit;)
				{
					if (Mask[N].Normal != 0)
					{
						const auto CurrentMask = Mask[N];
						ChunkItr[Axis1]        = i;
						ChunkItr[Axis2]        = j;

						int Width;

						for (Width = 1; i + Width < Axis1Limit && CompareMask(Mask[N + Width], CurrentMask); ++Width)
						{
						}

						int Height;
						bool Done = false;

						for (Height = 1; j + Height < Axis2Limit; ++Height)
						{
							for (int k = 0; k < Width; ++k)
							{
								if (CompareMask(Mask[N + k + Height * Axis1Limit], CurrentMask)) continue;

								Done = true;
								break;
							}

							if (Done) break;
						}

						DeltaAxis1[Axis1] = Width;
						DeltaAxis2[Axis2] = Height;

						CreateQuad_Static(
						                  CurrentMask
						                , AxisMask
						                , Width
						                , Height
						                , ChunkItr
						                , ChunkItr + DeltaAxis1
						                , ChunkItr + DeltaAxis2
						                , ChunkItr + DeltaAxis1 + DeltaAxis2
						                , InAsyncData.InUnitVoxelLength
						                , OutMeshData
						                 );

						DeltaAxis1 = FIdxVector::ZeroValue;
						DeltaAxis2 = FIdxVector::ZeroValue;

						for (int l = 0; l < Height; ++l)
						{
							for (int k = 0; k < Width; ++k)
							{
								Mask[N + k + l * Axis1Limit] = FMask{EBlockType::Null, 0};
							}
						}

						i += Width;
						N += Width;
					}
					else
					{
						i++;
						N++;
					}
				}
			}
		}
	}
}

void AGreedyChunk::CreateQuad_Static(const FMask InMask, const FIdxVector& InAxisMask, const int InWidth, const int InHeight, const FIdxVector& InV1, const FIdxVector& InV2, const FIdxVector& InV3, const FIdxVector& InV4, const float InUnitVoxelLength, FChunkMeshData& OutMeshData)
{
	
	const FVector Normal = FVector(InAxisMask * InMask.Normal);
	const FColor Color   = GetTextureIndexColor(InMask.BlockType, Normal);

	OutMeshData.Vertices.Append({
								 FVector(InV1) * InUnitVoxelLength
							   , FVector(InV2) * InUnitVoxelLength
							   , FVector(InV3) * InUnitVoxelLength
							   , FVector(InV4) * InUnitVoxelLength
							 });

	OutMeshData.Triangles.Append({
								  OutMeshData.VertexCount
								, OutMeshData.VertexCount + 2 + InMask.Normal
								, OutMeshData.VertexCount + 2 - InMask.Normal
								, OutMeshData.VertexCount + 3
								, OutMeshData.VertexCount + 1 - InMask.Normal
								, OutMeshData.VertexCount + 1 + InMask.Normal
							  });

	OutMeshData.Normals.Append({
								Normal
							  , Normal
							  , Normal
							  , Normal
							});

	OutMeshData.Colors.Append({
							   Color
							 , Color
							 , Color
							 , Color
						   });

	if (Normal.X == 1 || Normal.X == -1)
	{
		OutMeshData.UV0.Append({
								FVector2D(InWidth, InHeight)
							  , FVector2D(0, InHeight)
							  , FVector2D(InWidth, 0)
							  , FVector2D(0, 0)
							});
	}
	else
	{
		OutMeshData.UV0.Append({
								FVector2D(InHeight, InWidth)
							  , FVector2D(InHeight, 0)
							  , FVector2D(0, InWidth)
							  , FVector2D(0, 0)
							});
	}

	OutMeshData.VertexCount += 4;
}

bool AGreedyChunk::CompareMask(const FMask InM1, const FMask InM2)
{
	return InM1.BlockType == InM2.BlockType && InM1.Normal == InM2.Normal;
}
