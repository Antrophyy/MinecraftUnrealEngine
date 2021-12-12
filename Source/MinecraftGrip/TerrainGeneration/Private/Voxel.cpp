#include "MinecraftGrip/TerrainGeneration/Public/Voxel.h"
#include "MinecraftGrip/TerrainGeneration/Public/BlockType.h"
#include "MinecraftGrip/TerrainGeneration/Public/ChunkData.h"
#include "MinecraftGrip/TerrainGeneration/Public/GameWorld.h"
#include "MinecraftGrip/TerrainGeneration/Public/VoxelData.h"

EBlockType FVoxel::DetermineBlockTypeBasedOnHeight() const
{
	const int32 PositionZ = FMath::FloorToInt(PositionInChunk.Z);
	const int32 HighestSolidGroundLevelNotRounded = FMath::FloorToInt(4000 * FMath::PerlinNoise2D(FVector2D(
		(PositionInChunk.X + 4000 * AGameWorld::GetSeededPerlinNoiseNumber()) / (FChunkData::ChunkWidthSize + 4000),
		(PositionInChunk.Y + 4000 * AGameWorld::GetSeededPerlinNoiseNumber()) / (FChunkData::ChunkWidthSize + 4000)))) + 8000;

	const int32 HighestSolidGroundLevel = HighestSolidGroundLevelNotRounded - HighestSolidGroundLevelNotRounded % 100 + 100;

	EBlockType VoxelType = EBlockType::StoneBlock;

	if (!IsVoxelInWorld(PositionInChunk))
		return EBlockType::AirBlock;
	if (PositionZ < 500)
		return EBlockType::BedrockBlock;
	if (PositionZ > 9000 && PositionZ <= HighestSolidGroundLevel)
		return EBlockType::SnowBlock;
	if (PositionZ > 9000 - 500 && PositionZ == HighestSolidGroundLevel)
		return EBlockType::SnowDirtBlock;
	if (PositionZ == HighestSolidGroundLevel)
		VoxelType = EBlockType::GrassBlock;
	if (PositionZ < HighestSolidGroundLevel && PositionZ > HighestSolidGroundLevel - 500)
		VoxelType = EBlockType::DirtBlock;
	if (PositionZ > HighestSolidGroundLevel)
		return EBlockType::AirBlock;
	
	return GenerateBlockBelowGround(PositionZ, HighestSolidGroundLevel, VoxelType);
}


EBlockType FVoxel::GenerateBlockBelowGround(const int32 InCurrentPosition, const int32 InHighestBlockPosition, EBlockType InVoxelType) const
{
	const float Perlin = FMath::PerlinNoise3D((PositionInChunk / FChunkData::ChunkWidthSize + 0.1f) * 0.5f);
	if (InCurrentPosition >= 2000 && InCurrentPosition <= InHighestBlockPosition)
	{
		if (Perlin > 0.3f)
			return EBlockType::AirBlock;
	}

	const float PerlinDetails = FMath::PerlinNoise3D((PositionInChunk / FChunkData::ChunkWidthSize + 0.1f) * 0.8f);
	if (InVoxelType == EBlockType::StoneBlock)
	{
		if (PerlinDetails > 0.3f)
			InVoxelType = EBlockType::SandBlock;
		if (PerlinDetails > -0.2f && PerlinDetails < 0.2f)
			InVoxelType = EBlockType::DirtBlock;
	}

	return InVoxelType;
}

bool FVoxel::IsVoxelInWorld(const FVector& Position)
{
	return Position.X >= 0 && Position.X < FChunkData::WorldSizeInVoxels
		&& Position.Y >= 0 && Position.Y < FChunkData::WorldSizeInVoxels
		&& Position.Z >= 0 && Position.Z < FChunkData::ChunkHeightSize;
}

ETextureType FVoxel::GetTextureType(const int32 InSideIndex) const
{
	switch (InSideIndex)
	{
	case 0:
		return BottomTexture;
	case 1:
		return SideTexture;
	case 2:
		return TopTexture;
	case 3:
		return SideTexture;
	case 4:
		return SideTexture;
	case 5:
		return SideTexture;
	default:
		return SideTexture;
	}
}

void FVoxel::AssignPropertiesBasedOnType()
{
	for (const FVoxel Voxel : FVoxelData::VoxelTypes)
	{
		if (Voxel.BlockType == BlockType)
		{
			bIsSolid = Voxel.bIsSolid;
			DestroyTime = Voxel.DestroyTime;
			SideTexture = Voxel.SideTexture;
			TopTexture = Voxel.TopTexture;
			BottomTexture = Voxel.BottomTexture;
		}
	}
}

void FVoxel::InitializeVoxel(const FVector& InPosition)
{
	PositionInChunk = InPosition;

	BlockType = DetermineBlockTypeBasedOnHeight();
	AssignPropertiesBasedOnType();
}

void FVoxel::InitializeVoxelType(const bool InbIsSolid, const EBlockType InBlockType, const float InDestroyTime,
                                 const ETextureType InSideTexture, const ETextureType InTopTexture,
                                 const ETextureType InBottomTexture)
{
	SideTexture = InSideTexture;
	TopTexture = InTopTexture;
	BottomTexture = InBottomTexture;
	bIsSolid = InbIsSolid;
	DestroyTime = InDestroyTime;
	BlockType = InBlockType;
}

void FVoxel::InitializeSpecificVoxel(const FVector& InPosition, const EBlockType InBlockType)
{
	PositionInChunk = InPosition;
	BlockType = InBlockType;

	AssignPropertiesBasedOnType();
}

void FVoxel::ModifyVoxel(const EBlockType NewBlockType)
{
	BlockType = NewBlockType;
	AssignPropertiesBasedOnType();
}

bool FVoxel::IsVoxelOutsideChunk(const FIntVector& Position)
{
	return Position.X < 0 || Position.X > FChunkData::ChunkWidthSize
		|| Position.Y < 0 || Position.Y > FChunkData::ChunkWidthSize
		|| Position.Z < 0 || Position.Z > FChunkData::ChunkHeightSize;
}
