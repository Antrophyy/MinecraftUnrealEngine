#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "GameFramework/Actor.h"

#include "GameWorld.generated.h"

struct FMeshData;
struct FChunkDetails;
class AMinecraftMode;
class AChunk;

/**
 *  Structure that helps create a 2D Array of chunks
 */
USTRUCT()
struct FChunkWidth
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AChunk*> ChunkWidthArray;
};

/**
 *  Class that handles the world generation and chunk loading.
 */
UCLASS()
class MINECRAFTGRIP_API AGameWorld final : public AActor
{
	GENERATED_BODY()

public:
	AGameWorld();
	virtual void Tick(const float DeltaTime) override;

public: // Methods used to help with population of the world.
	// Starts populating the game world with chunks and spawns a player.
	UFUNCTION(BlueprintCallable)
	void InitializeGameWorld();

	FORCEINLINE static int32 GetSeededPerlinNoiseNumber() { return SeededPerlinNoiseNumber; }
	FORCEINLINE static TQueue<FMeshData>& GetChunkSpawnerQueue() { return ChunkSpawnerQueue; }
	bool VoxelExistsAndIsSolid(const FVector& Position);
	AChunk* GetChunkFromWorldPosition(const FVector& Position);

	// Sets the custom seed for the generated world (or a loaded world). 
	UFUNCTION(BlueprintCallable)
	void SetCustomSeed(const int64 InCustomSeed);

public: // Methods used for saving and loading the game
	FORCEINLINE const TArray<FChunkWidth>& GetChunkArray() const { return ChunkArray; }
	
	// Reloads the game world with the loaded data from save file.
	void SetChunkArray(const TArray<struct FChunkDetails>& InChunkArrayDetails);

	FORCEINLINE const TArray<FVector2D>& GetActiveChunksArray() const { return ActiveChunks; }
	FORCEINLINE void SetActiveChunkArray(const TArray<FVector2D>& InActiveChunkArray) { ActiveChunks = InActiveChunkArray; }
	FORCEINLINE int64 GetCustomSeed() const { return CustomSeed; }

protected:
	virtual void BeginPlay() override;

private: // Attributes for generated world.
	UPROPERTY()
	TArray<FChunkWidth> ChunkArray;

	// Used for loading bar.
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	int32 ChunksSpawnedCounter;

	TArray<FVector2D> ActiveChunks;
	static TQueue<FMeshData> ChunkSpawnerQueue;

private: // Attributes for visibility settings. 
	int32 ViewDistanceInChunks;
	FVector2D PlayerPositionChunk;
	FVector2D PlayerPreviousPositionChunk;
	float SpawnChunkDelay;
	int32 NumberOfChunksAllowed;

private: // Attributes for world generation.
	static FRandomStream RandomStream;
	static int32 SeededPerlinNoiseNumber;
	int64 CustomSeed;

private: // World generation methods
	void InitializeChunkArray();
	void GenerateChunks();
	void HandleChunkDelayedSpawner(const float DeltaTime);
	void RemovePreviousChunks();

private: // Handle player moving around the world.
	void CheckViewDistance();
	void MovePawnToChunkArrayCenter() const;
	void UpdatePlayerPositionAndViewDistance();
};
