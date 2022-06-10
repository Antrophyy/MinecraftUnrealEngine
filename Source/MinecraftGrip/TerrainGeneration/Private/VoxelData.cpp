#include "MinecraftGrip/TerrainGeneration/Public/VoxelData.h"
#include "MinecraftGrip/TerrainGeneration/Public/BlockType.h"
#include "MinecraftGrip/TerrainGeneration/Public/Voxel.h"

TArray<FIntVector> FVoxelData::VoxelVertices
{
	FIntVector{0, 0, 0},
	FIntVector{100, 0, 0},
	FIntVector{100, 100, 0},
	FIntVector{0, 100, 0},
	FIntVector{0, 0, 100},
	FIntVector{100, 0, 100},
	FIntVector{100, 100, 100},
	FIntVector{0, 100, 100}
};

TArray<FSecondDimensionArray> FVoxelData::VoxelSides
{
	FSecondDimensionArray{TArray<int32>{2, 1, 0, 3}}, // Bottom
	FSecondDimensionArray{TArray<int32>{7, 3, 0, 4}}, // Front
	FSecondDimensionArray{TArray<int32>{4, 5, 6, 7}}, // Top
	FSecondDimensionArray{TArray<int32>{6, 2, 3, 7}}, // Right
	FSecondDimensionArray{TArray<int32>{4, 0, 1, 5}}, // Left 
	FSecondDimensionArray{TArray<int32>{5, 1, 2, 6}}, // Back
};

TArray<FIntVector> FVoxelData::SideChecks
{
	FIntVector{0, 0, -100},
	FIntVector{-100, 0, 0},
	FIntVector{0, 0, 100},
	FIntVector{0, 100, 0},
	FIntVector{0, -100, 0},
	FIntVector{100, 0, 0},
};

TMap<EMinecraftTextureType, TArray<FVector2D>> FVoxelData::TextureTypeUVMapping
{
	{
		EMinecraftTextureType::StoneTexture, TArray<FVector2D>{
			FVector2D(0.25f, 0.f),
			FVector2D(0.f, 0.f),
			FVector2D(0.f, 0.25f),
			FVector2D(0.25f, 0.25f)
		},
	},
	{
		EMinecraftTextureType::DirtTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.f),
			FVector2D(0.25f, 0.f),
			FVector2D(0.25f, 0.25f),
			FVector2D(0.5f, 0.25f)
		}
	},
	{
		EMinecraftTextureType::GrassDirtTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.f),
			FVector2D(0.5f, 0.25f),
			FVector2D(0.75f, 0.25f),
			FVector2D(0.75f, 0.f)
		}
	},
	{
		EMinecraftTextureType::SnowDirtTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.f),
			FVector2D(0.75f, 0.25f),
			FVector2D(1.f, 0.25f),
			FVector2D(1.f, 0.f)
		}
	},
	{
		EMinecraftTextureType::SnowTexture, TArray<FVector2D>{
			FVector2D(0.25f, 0.25f),
			FVector2D(0.f, 0.25f),
			FVector2D(0.f, 0.5f),
			FVector2D(0.25f, 0.5f)
		}
	},
	{
		EMinecraftTextureType::WoodSideTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.25f),
			FVector2D(0.25f, 0.25f),
			FVector2D(0.25f, 0.5f),
			FVector2D(0.5f, 0.5f)
		}
	},
	{
		EMinecraftTextureType::WoodTopBottomTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.25f),
			FVector2D(0.5f, 0.25f),
			FVector2D(0.5f, 0.5f),
			FVector2D(0.75f, 0.5f)
		}
	},
	{
		EMinecraftTextureType::GrassTopTexture, TArray<FVector2D>{
			FVector2D(1.f, 0.25f),
			FVector2D(0.75f, 0.25f),
			FVector2D(0.75f, 0.5f),
			FVector2D(1.f, 0.5f)
		}
	},
	{
		EMinecraftTextureType::BedrockTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.5f),
			FVector2D(0.25f, 0.5f),
			FVector2D(0.25f, 0.75f),
			FVector2D(0.5f, 0.75f)
		}
	},
	{
		EMinecraftTextureType::SandTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.5f),
			FVector2D(0.5f, 0.5f),
			FVector2D(0.5f, 0.75f),
			FVector2D(0.75f, 0.75f)
		}
	},
	{
		EMinecraftTextureType::GreenLeafTexture, TArray<FVector2D>{
			FVector2D(1.f, 0.5f),
			FVector2D(0.75f, 0.5f),
			FVector2D(0.75f, 0.75f),
			FVector2D(1.f, 0.75f)
		}
	},
	{
		EMinecraftTextureType::BirchTopBottomTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.75f),
			FVector2D(0.5f, 0.75f),
			FVector2D(0.5f, 1.f),
			FVector2D(0.75f, 1.f)
		}
	},
	{
		EMinecraftTextureType::BirchSideTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.75f),
			FVector2D(0.5f, 0.75f),
			FVector2D(0.5f, 1.f),
			FVector2D(0.75f, 1.f)
		}
	},
	{
		EMinecraftTextureType::BrownLeafTexture, TArray<FVector2D>{
			FVector2D(1.f, 0.75f),
			FVector2D(0.75f, 0.75f),
			FVector2D(0.75f, 1.f),
			FVector2D(1.f, 1.f)
		}
	}
};

TArray<FVoxel> FVoxelData::VoxelTypes
{
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::BedrockTexture,
		EMinecraftTextureType::BedrockTexture,
		EMinecraftTextureType::BedrockTexture,
		false,
		0.f,
		EBlockType::AirBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::BedrockTexture,
		EMinecraftTextureType::BedrockTexture,
		EMinecraftTextureType::BedrockTexture,
		true,
		100.f,
		EBlockType::BedrockBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::StoneTexture,
		EMinecraftTextureType::StoneTexture,
		EMinecraftTextureType::StoneTexture,
		true,
		3.f,
		EBlockType::StoneBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::GrassDirtTexture,
		EMinecraftTextureType::GrassTopTexture,
		EMinecraftTextureType::DirtTexture,
		true,
		2.f,
		EBlockType::GrassBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::DirtTexture,
		EMinecraftTextureType::DirtTexture,
		EMinecraftTextureType::DirtTexture,
		true,
		1.5f,
		EBlockType::DirtBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::SandTexture,
		EMinecraftTextureType::SandTexture,
		EMinecraftTextureType::SandTexture,
		true,
		2.f,
		EBlockType::SandBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::SnowDirtTexture,
		EMinecraftTextureType::SnowTexture,
		EMinecraftTextureType::DirtTexture,
		true,
		1.8f,
		EBlockType::SnowDirtBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::SnowTexture,
		EMinecraftTextureType::SnowTexture,
		EMinecraftTextureType::SnowTexture,
		true,
		0.5f,
		EBlockType::SnowBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::WoodSideTexture,
		EMinecraftTextureType::WoodTopBottomTexture,
		EMinecraftTextureType::WoodTopBottomTexture,
		true,
		3.f,
		EBlockType::WoodBlock
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::BirchSideTexture,
		EMinecraftTextureType::BirchTopBottomTexture,
		EMinecraftTextureType::BirchTopBottomTexture,
		true,
		3.f,
		EBlockType::BirchWood
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::GreenLeafTexture,
		EMinecraftTextureType::GreenLeafTexture,
		EMinecraftTextureType::GreenLeafTexture,
		true,
		0.2f,
		EBlockType::GreenLeaves
	},
	FVoxel
	{
		FVector{},
		EMinecraftTextureType::BrownLeafTexture,
		EMinecraftTextureType::BrownLeafTexture,
		EMinecraftTextureType::BrownLeafTexture,
		true,
		0.2f,
		EBlockType::BrownLeaves
	},
};