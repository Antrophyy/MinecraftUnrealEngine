#pragma once

#include "TextureType.h"

#include "CoreMinimal.h"

#include "VoxelData.generated.h"

struct FVoxel;

/**
 * Structure used to help create a 2D array of ints.
 */
struct FSecondDimensionArray
{
	TArray<int32> InnerInts;
};

/**
 * Structure that contains definite voxel data.
 */
USTRUCT()
struct FVoxelData
{
	GENERATED_BODY()

	// Different voxel types with its default parameters.
	static TArray<FVoxel> VoxelTypes;
	static TArray<FIntVector> VoxelVertices;
	static TArray<FSecondDimensionArray> VoxelSides;
	static TArray<FIntVector> SideChecks;
	
	// Every block has different textures for their sides, this will map the texture to specific sides.
	static TMap<EMinecraftTextureType, TArray<FVector2D>> TextureTypeUVMapping;
};
