#pragma once

#include "MinecraftGrip/TerrainGeneration/Public/Voxel.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "SaveMinecraftGame.generated.h"

// Struct that is used for saving the modified voxels for every chunk with its coordinates.
USTRUCT()
struct FChunkDetails
{
	GENERATED_BODY()

	UPROPERTY()
	FIntVector Coordinates;

	// The modified voxels.
	UPROPERTY()
	TMap<FIntVector, FVoxel> VoxelMapModifications;
};

/**
 * Class used for saving the game state. 
 */
UCLASS()
class MINECRAFTGRIP_API USaveMinecraftGame final : public USaveGame
{
	GENERATED_BODY()

public: // Getters and setters needed for saving the game.
	FORCEINLINE const TArray<FChunkDetails>& GetGameMap() const { return GameMap; }
	FORCEINLINE void SetGameMap(const TArray<FChunkDetails>& InGameMap) { GameMap = InGameMap; }

	FORCEINLINE const TArray<FVector2D>& GetActiveChunksCoordinates() const { return ActiveChunksCoordinates; }
	FORCEINLINE void SetActiveChunksCoordinates(const TArray<FVector2D>& InActiveChunksCoordinates) { ActiveChunksCoordinates = InActiveChunksCoordinates; }

	FORCEINLINE const FVector& GetPlayerPosition() const { return PlayerPosition; }
	FORCEINLINE void SetPlayerPosition(const FVector& InPlayerPosition) { PlayerPosition = InPlayerPosition; }

	FORCEINLINE int32 GetCustomSeed() const { return CustomSeed; }
	FORCEINLINE void SetCustomSeed(const int32 InCustomSeed) { CustomSeed = InCustomSeed; }

private: // Variables that are initialized from the game after pressing the save button.
	UPROPERTY()
	TArray<FChunkDetails> GameMap;
	
	UPROPERTY()
	TArray<FVector2D> ActiveChunksCoordinates;
	
	UPROPERTY()
	FVector PlayerPosition;
	
	UPROPERTY()
	int32 CustomSeed;


};
