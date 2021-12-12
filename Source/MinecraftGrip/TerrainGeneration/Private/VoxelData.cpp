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

TMap<ETextureType, TArray<FVector2D>> FVoxelData::TextureTypeUVMapping
{
	{
		ETextureType::StoneTexture, TArray<FVector2D>{
			FVector2D(0.25f, 0.f),
			FVector2D(0.f, 0.f),
			FVector2D(0.f, 0.25f),
			FVector2D(0.25f, 0.25f)
		},
	},
	{
		ETextureType::DirtTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.f),
			FVector2D(0.25f, 0.f),
			FVector2D(0.25f, 0.25f),
			FVector2D(0.5f, 0.25f)
		}
	},
	{
		ETextureType::GrassDirtTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.f),
			FVector2D(0.5f, 0.25f),
			FVector2D(0.75f, 0.25f),
			FVector2D(0.75f, 0.f)
		}
	},
	{
		ETextureType::SnowDirtTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.f),
			FVector2D(0.75f, 0.25f),
			FVector2D(1.f, 0.25f),
			FVector2D(1.f, 0.f)
		}
	},
	{
		ETextureType::SnowTexture, TArray<FVector2D>{
			FVector2D(0.25f, 0.25f),
			FVector2D(0.f, 0.25f),
			FVector2D(0.f, 0.5f),
			FVector2D(0.25f, 0.5f)
		}
	},
	{
		ETextureType::WoodSideTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.25f),
			FVector2D(0.25f, 0.25f),
			FVector2D(0.25f, 0.5f),
			FVector2D(0.5f, 0.5f)
		}
	},
	{
		ETextureType::WoodTopBottomTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.25f),
			FVector2D(0.5f, 0.25f),
			FVector2D(0.5f, 0.5f),
			FVector2D(0.75f, 0.5f)
		}
	},
	{
		ETextureType::GrassTopTexture, TArray<FVector2D>{
			FVector2D(1.f, 0.25f),
			FVector2D(0.75f, 0.25f),
			FVector2D(0.75f, 0.5f),
			FVector2D(1.f, 0.5f)
		}
	},
	{
		ETextureType::BedrockTexture, TArray<FVector2D>{
			FVector2D(0.5f, 0.5f),
			FVector2D(0.25f, 0.5f),
			FVector2D(0.25f, 0.75f),
			FVector2D(0.5f, 0.75f)
		}
	},
	{
		ETextureType::SandTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.5f),
			FVector2D(0.5f, 0.5f),
			FVector2D(0.5f, 0.75f),
			FVector2D(0.75f, 0.75f)
		}
	},
	{
		ETextureType::GreenLeafTexture, TArray<FVector2D>{
			FVector2D(1.f, 0.5f),
			FVector2D(0.75f, 0.5f),
			FVector2D(0.75f, 0.75f),
			FVector2D(1.f, 0.75f)
		}
	},
	{
		ETextureType::BirchTopBottomTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.75f),
			FVector2D(0.5f, 0.75f),
			FVector2D(0.5f, 1.f),
			FVector2D(0.75f, 1.f)
		}
	},
	{
		ETextureType::BirchSideTexture, TArray<FVector2D>{
			FVector2D(0.75f, 0.75f),
			FVector2D(0.5f, 0.75f),
			FVector2D(0.5f, 1.f),
			FVector2D(0.75f, 1.f)
		}
	},
	{
		ETextureType::BrownLeafTexture, TArray<FVector2D>{
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
		ETextureType::BedrockTexture,
		ETextureType::BedrockTexture,
		ETextureType::BedrockTexture,
		false,
		0.f,
		EBlockType::AirBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::BedrockTexture,
		ETextureType::BedrockTexture,
		ETextureType::BedrockTexture,
		true,
		100.f,
		EBlockType::BedrockBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::StoneTexture,
		ETextureType::StoneTexture,
		ETextureType::StoneTexture,
		true,
		3.f,
		EBlockType::StoneBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::GrassDirtTexture,
		ETextureType::GrassTopTexture,
		ETextureType::DirtTexture,
		true,
		2.f,
		EBlockType::GrassBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::DirtTexture,
		ETextureType::DirtTexture,
		ETextureType::DirtTexture,
		true,
		1.5f,
		EBlockType::DirtBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::SandTexture,
		ETextureType::SandTexture,
		ETextureType::SandTexture,
		true,
		2.f,
		EBlockType::SandBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::SnowDirtTexture,
		ETextureType::SnowTexture,
		ETextureType::DirtTexture,
		true,
		1.8f,
		EBlockType::SnowDirtBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::SnowTexture,
		ETextureType::SnowTexture,
		ETextureType::SnowTexture,
		true,
		0.5f,
		EBlockType::SnowBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::WoodSideTexture,
		ETextureType::WoodTopBottomTexture,
		ETextureType::WoodTopBottomTexture,
		true,
		3.f,
		EBlockType::WoodBlock
	},
	FVoxel
	{
		FVector{},
		ETextureType::BirchSideTexture,
		ETextureType::BirchTopBottomTexture,
		ETextureType::BirchTopBottomTexture,
		true,
		3.f,
		EBlockType::BirchWood
	},
	FVoxel
	{
		FVector{},
		ETextureType::GreenLeafTexture,
		ETextureType::GreenLeafTexture,
		ETextureType::GreenLeafTexture,
		true,
		0.2f,
		EBlockType::GreenLeaves
	},
	FVoxel
	{
		FVector{},
		ETextureType::BrownLeafTexture,
		ETextureType::BrownLeafTexture,
		ETextureType::BrownLeafTexture,
		true,
		0.2f,
		EBlockType::BrownLeaves
	},
};