#include "MinecraftGrip/TerrainGeneration/Public/ChunkData.h"

bool FChunkData::IsChunkInWorld(const FVector2D& InChunkCoordinates)
{
	return InChunkCoordinates.X >= 0 && InChunkCoordinates.X < WorldSizeInChunks
		&& InChunkCoordinates.Y >= 0 && InChunkCoordinates.Y < WorldSizeInChunks;
}

FVector2D FChunkData::GetChunkPositionFromVector(const FVector& WorldPosition)
{
	return FVector2D(FMath::FloorToInt(WorldPosition.X / ChunkWidthSize), FMath::FloorToInt(WorldPosition.Y / ChunkWidthSize));
}
