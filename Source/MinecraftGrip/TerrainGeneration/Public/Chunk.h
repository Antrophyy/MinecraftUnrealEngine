#pragma once

#include "BlockType.h"
#include "Voxel.h"

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Chunk.generated.h"

struct FChunkWidth;

/**
 * The class manages data about the chunk and its voxels.
 */
UCLASS()
class MINECRAFTGRIP_API AChunk final : public AActor
{
	GENERATED_BODY()

public:
	AChunk();
	virtual void Tick(const float DeltaTime) override;

public: // Chunk modifications
	// Creates and initializes the chunk. 
	void InitializeChunk(AGameWorld* InGameWorld, const FIntVector& InSpawnLocation);

	// Method renders chunk to the world to be visible.
	void RenderChunk(const TArray<FVector>& InVertices, const TArray<int32>& InTriangles, const TArray<FVector2D>& InUVs);

	// Method updates a voxel type at a given position.
	void UpdateChunkWithVoxel(const FVector& Position, const EBlockType InVoxelType);

	// Method sets all voxels in the radius around passed position to type air (i.e. removes all voxels in the radius starting from passed position).
	void MakeExplosionAt(const FVector& InPositionOfInitialExplosion, int32 InRadius);

	// Forces the chunk to be rebuilt and re-rendered.
	void RebuildChunk();

	// Hides the chunk from the world and disables its collision.
	void SetChunkIsActive(const bool bIsActive);

public: // Methods for returning chunk data.

	// Returns the voxel in this chunk that is at the passed position.
	FVoxel GetVoxelFromGlobalFVector(const FVector& InPosition) const;

	// Converts FVector to a floored int vector
	static FIntVector FVectorToFlooredVector(const FVector& InFVector);

	FORCEINLINE bool GetVoxelMapPopulated() const { return bIsVoxelMapPopulated; }
	FORCEINLINE void SetVoxelMapPopulated(const bool bVoxelMapPopulated) { bIsVoxelMapPopulated = bVoxelMapPopulated; }

	FORCEINLINE const TMap<FIntVector, FVoxel>& GetVoxelMap() const { return VoxelMap; }
	FORCEINLINE void SetVoxelMap(const TMap<FIntVector, FVoxel>& InVoxelMap) { VoxelMap = InVoxelMap; }

	FORCEINLINE const TMap<FIntVector, FVoxel>& GetVoxelMapModifications() const { return VoxelMapModifications; }
	FORCEINLINE void SetVoxelMapModifications(const TMap<FIntVector, FVoxel>& InVoxelMapModifications) { VoxelMapModifications = InVoxelMapModifications; }

	FORCEINLINE const FIntVector& GetSpawnLocation() const { return SpawnLocation; }

protected:
	virtual void BeginPlay() override;

private: // Handles chunk updates
	void UpdateSurroundingChunks(const FIntVector& InVoxelPosition) const;
	void ModifyVoxel(const FIntVector& InPosition, const EBlockType InVoxelType);
	void LoadChunkAsync();


private: // Attributes for keeping chunk data
	TMap<FIntVector, FVoxel> VoxelMap;
	FIntVector SpawnLocation;
	bool bIsVoxelMapPopulated;
	uint8 RenderFlip;

	UPROPERTY()
	TMap<FIntVector, FVoxel> VoxelMapModifications;

	UPROPERTY()
	UProceduralMeshComponent* ProceduralMeshComponent;

private: // Attributes for basic need
	UPROPERTY()
	class AGameWorld* GameWorld;
};
