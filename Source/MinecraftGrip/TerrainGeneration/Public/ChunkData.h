#pragma once

#include "CoreMinimal.h"

#include "ChunkData.generated.h"

// Structure that contains the relevant data for any chunk
USTRUCT()
struct FChunkData
{
	GENERATED_BODY()
	
	static constexpr int32 ChunkWidthSize = 1600;
	static constexpr int32 ChunkHeightSize = 12800;
	static constexpr int32 WorldSizeInChunks = 500;
	static constexpr int32 WorldSizeInVoxels = WorldSizeInChunks * ChunkWidthSize;
	static bool IsChunkInWorld(const FVector2D& InChunkCoordinates);
	static FVector2D GetChunkPositionFromVector(const FVector& WorldPosition);
};
