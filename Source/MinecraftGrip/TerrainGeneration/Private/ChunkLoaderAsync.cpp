#include "MinecraftGrip/TerrainGeneration/Public/ChunkLoaderAsync.h"
#include "MinecraftGrip/TerrainGeneration/Public/Chunk.h"
#include "MinecraftGrip/TerrainGeneration/Public/ChunkData.h"
#include "MinecraftGrip/TerrainGeneration/Public/GameWorld.h"
#include "MinecraftGrip/TerrainGeneration/Public/VoxelData.h"

FChunkLoaderAsync::FChunkLoaderAsync(const FIntVector& InWorldPosition,
                                     const TMap<FIntVector, FVoxel>& InVoxelMap,
                                     const TMap<FIntVector, FVoxel>& InModifiedVoxelMap,
                                     const TArray<FChunkWidth>& InChunkArray):
	SpawnPosition(InWorldPosition),
	VertexIndex(0)
{
	MeshData.VoxelMap = InVoxelMap;
	ModifiedVoxelMap = InModifiedVoxelMap;
	MeshData.XCoord = InWorldPosition.X / FChunkData::ChunkWidthSize;
	MeshData.YCoord = InWorldPosition.Y / FChunkData::ChunkWidthSize;
	ChunkArray = InChunkArray;
}

TStatId FChunkLoaderAsync::GetStatId()
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FChunkLoaderAsync, STATGROUP_ThreadPoolAsyncTasks);
}

void FChunkLoaderAsync::DoWork()
{
	PopulateVoxelMap();
	GenerateVertices();

	AGameWorld::GetChunkSpawnerQueue().Enqueue(MeshData);
}

void FChunkLoaderAsync::PopulateVoxelMap()
{
	if (MeshData.VoxelMap.Num() > 0)
		return;

	for (TTuple<FIntVector, FVoxel> ModifiedVoxel : ModifiedVoxelMap)
		MeshData.VoxelMap.Add(ModifiedVoxel.Key, ModifiedVoxel.Value);

	for (int32 x = 0; x < FChunkData::ChunkWidthSize; x += 100)
	{
		for (int32 y = 0; y < FChunkData::ChunkWidthSize; y += 100)
		{
			for (int32 z = 0; z < FChunkData::ChunkHeightSize; z += 100)
			{
				if (!MeshData.VoxelMap.Contains(FIntVector{x, y, z}))
				{
					FVoxel Voxel;
					Voxel.InitializeVoxel(FVector{FIntVector{x, y, z}} + FVector{SpawnPosition});
					MeshData.VoxelMap.Add(FIntVector(x, y, z), Voxel);

					if (x <= FChunkData::ChunkWidthSize - 300 && x >= 300 && y <= 300 && y >= 300
						&& MeshData.VoxelMap.FindRef(FIntVector(x, y, z)).BlockType == EBlockType::GrassBlock)
					{
						const int32 RandomWillTreeBeGenerated = FMath::RandRange(1, 100);
						if (RandomWillTreeBeGenerated >= 95)
						{
							FVoxel TreeInitialVoxel;
							const EBlockType WoodType = static_cast<EBlockType>(FMath::RandRange(8, 9));
							TreeInitialVoxel.InitializeSpecificVoxel(FVector(x, y, z), WoodType);
							MeshData.VoxelMap.Add(FIntVector(x, y, z), TreeInitialVoxel);

							GenerateTreeAtLocation(FIntVector(x, y, z), WoodType);
						}
					}
				}
			}
		}
	}
}

void FChunkLoaderAsync::GenerateVertices()
{
	for (int32 x = 0; x < FChunkData::ChunkWidthSize; x += 100)
	{
		for (int32 y = 0; y < FChunkData::ChunkWidthSize; y += 100)
		{
			for (int32 z = 0; z < FChunkData::ChunkHeightSize; z += 100)
			{
				FIntVector Position{x, y, z};
				if (MeshData.VoxelMap.FindRef(Position).BlockType != EBlockType::AirBlock)
					UpdateMeshData(Position);
			}
		}
	}
}

void FChunkLoaderAsync::UpdateMeshData(const FIntVector& InPosition)
{
	for (int i = 0; i < 6; ++i)
	{
		if (!IsVoxelSideOccupied(FVector(InPosition + FVoxelData::SideChecks[i])))
		{
			for (int p = 0; p < 4; ++p)
				MeshData.Vertices.Add(FVector{InPosition + FVoxelData::VoxelVertices[FVoxelData::VoxelSides[i].InnerInts[p]]});

			const int32 BlockID = static_cast<int32>(MeshData.VoxelMap.FindRef(InPosition).BlockType);

			MeshData.Triangles.Add(VertexIndex + 1);
			MeshData.Triangles.Add(VertexIndex);
			MeshData.Triangles.Add(VertexIndex + 2);
			MeshData.Triangles.Add(VertexIndex + 2);
			MeshData.Triangles.Add(VertexIndex);
			MeshData.Triangles.Add(VertexIndex + 3);

			const EMinecraftTextureType TextureType = FVoxelData::VoxelTypes[BlockID].GetTextureType(i);
			AddTexture(TextureType);

			VertexIndex += 4;
		}
	}
}

void FChunkLoaderAsync::AddTexture(const EMinecraftTextureType InTextureType)
{
	MeshData.UVs.Append(FVoxelData::TextureTypeUVMapping.FindRef(InTextureType));
}

bool FChunkLoaderAsync::IsVoxelSideOccupied(const FVector& InPosition) const
{
	const FIntVector NewPos = AChunk::FVectorToFlooredVector(InPosition);

	if (FVoxel::IsVoxelOutsideChunk(NewPos))
		return VoxelExistsAndIsSolid(FVector(NewPos) + FVector{SpawnPosition});

	return FVoxelData::VoxelTypes[static_cast<int32>(MeshData.VoxelMap.FindRef(NewPos).BlockType)].bIsSolid;
}

bool FChunkLoaderAsync::VoxelExistsAndIsSolid(const FVector& InPosition) const
{
	const FVector2D ThisChunkPosition(FMath::FloorToInt(InPosition.X) / FChunkData::ChunkWidthSize,
	                                  FMath::FloorToInt(InPosition.Y) / FChunkData::ChunkWidthSize);

	if (!FChunkData::IsChunkInWorld(ThisChunkPosition) || InPosition.Z < 0 || InPosition.Z > FChunkData::ChunkHeightSize)
		return false;

	if (ChunkArray[ThisChunkPosition.X].ChunkWidthArray[ThisChunkPosition.Y] != nullptr
		&& ChunkArray[ThisChunkPosition.X].ChunkWidthArray[ThisChunkPosition.Y]->GetVoxelMapPopulated())
		return FVoxelData::VoxelTypes[static_cast<int>(ChunkArray[ThisChunkPosition.X].ChunkWidthArray[ThisChunkPosition.Y]->GetVoxelFromGlobalFVector(InPosition).BlockType)]
			.bIsSolid;

	FVoxel Voxel;
	Voxel.InitializeVoxel(InPosition);
	return FVoxelData::VoxelTypes[static_cast<int>(Voxel.BlockType)].bIsSolid;
}

FVoxel FChunkLoaderAsync::GetVoxelFromWorldPosition(const FVector& InPosition) const
{
	FIntVector NewPost = AChunk::FVectorToFlooredVector(InPosition);

	NewPost.X -= FMath::FloorToInt((float)SpawnPosition.X);
	NewPost.Y -= FMath::FloorToInt((float)SpawnPosition.Y);

	if (MeshData.VoxelMap.Contains(NewPost))
		return MeshData.VoxelMap.FindRef(NewPost);

	return MeshData.VoxelMap.FindRef(FIntVector{0, 0, 0});
}

void FChunkLoaderAsync::GenerateTreeTop(const FVector& InPosition, const EBlockType InTreeTopType)
{
	constexpr int32 Width = 400;
	for (int32 OffsetX = -(Width / 2); OffsetX <= Width / 2; OffsetX += 100)
	{
		for (int32 OffsetY = -(Width / 2); OffsetY <= Width / 2; OffsetY += 100)
		{
			for (int32 OffsetZ = -200; OffsetZ <= 100; OffsetZ += 100)
			{
				if (FMath::RandRange(0,10) > 4)
				{
					FVector LeafVoxelPosition{InPosition.X + OffsetX, InPosition.Y + OffsetY, InPosition.Z + OffsetZ};
					GenerateLeafAtPosition(LeafVoxelPosition, InTreeTopType);	
				}
			}
		}
	}
}

void FChunkLoaderAsync::GenerateLeafAtPosition(const FVector& InLocation, const EBlockType InTreeTopType)
{
	FVoxel LeafVoxel;
	LeafVoxel.InitializeSpecificVoxel(InLocation, InTreeTopType);
	MeshData.VoxelMap.Add(FIntVector(InLocation), LeafVoxel);
}

void FChunkLoaderAsync::GenerateTreeAtLocation(const FIntVector& InLocation, const EBlockType TreeTrunkType)
{
	const int32 MaxHeight = FMath::RandRange(4, 8) * 100;

	const EBlockType LeafType = TreeTrunkType == EBlockType::WoodBlock ? EBlockType::GreenLeaves : EBlockType::BrownLeaves;

	for (int32 TreeHeight = 100; TreeHeight <= MaxHeight; TreeHeight += 100)
	{
		FVoxel TreeTrunkVoxel;
		FVector VoxelLocation(InLocation.X, InLocation.Y, InLocation.Z + TreeHeight);
		TreeTrunkVoxel.InitializeSpecificVoxel(VoxelLocation, TreeTrunkType);
		MeshData.VoxelMap.Add(FIntVector{VoxelLocation}, TreeTrunkVoxel);

		if (TreeHeight == MaxHeight)
			GenerateTreeTop(VoxelLocation, LeafType);
	}
}
