#include "MinecraftGrip/TerrainGeneration/Public/GameWorld.h"
#include "MinecraftGrip/Minecraft/Public/SaveMinecraftGame.h"
#include "MinecraftGrip/TerrainGeneration/Public/Chunk.h"
#include "MinecraftGrip/TerrainGeneration/Public/ChunkData.h"
#include "MinecraftGrip/TerrainGeneration/Public/ChunkLoaderAsync.h"
#include "MinecraftGrip/TerrainGeneration/Public/VoxelData.h"

#include "Kismet/GameplayStatics.h"

TQueue<FMeshData> AGameWorld::ChunkSpawnerQueue;
int32 AGameWorld::SeededPerlinNoiseNumber;
FRandomStream AGameWorld::RandomStream;

AGameWorld::AGameWorld() :
	ViewDistanceInChunks(10),
	PlayerPreviousPositionChunk(FVector2D(FChunkData::WorldSizeInChunks / 2 - ViewDistanceInChunks,
	                                      FChunkData::WorldSizeInChunks / 2 + ViewDistanceInChunks)),
	NumberOfChunksAllowed(400)
{
	PrimaryActorTick.bCanEverTick = true;
	RandomStream = FRandomStream();
}

void AGameWorld::BeginPlay()
{
	Super::BeginPlay();
}

void AGameWorld::InitializeGameWorld()
{
	SetTickableWhenPaused(true);

	RemovePreviousChunks();
	InitializeChunkArray();
	GenerateChunks();
	MovePawnToChunkArrayCenter();
}

void AGameWorld::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerPositionAndViewDistance();
	HandleChunkDelayedSpawner(DeltaTime);
}

void AGameWorld::InitializeChunkArray()
{
	ChunkSpawnerQueue.Empty();
	for (int x = 0; x < FChunkData::WorldSizeInChunks; ++x)
	{
		ChunkArray.Add(FChunkWidth{});
		for (int y = 0; y < FChunkData::WorldSizeInChunks; ++y)
			ChunkArray[x].ChunkWidthArray.Add(nullptr);
	}
}

void AGameWorld::CheckViewDistance()
{
	const FVector2D ChunkCoordinates = FChunkData::GetChunkPositionFromVector(
		GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	
	PlayerPreviousPositionChunk = PlayerPositionChunk;

	TArray<FVector2D> PreviouslyActiveChunks{ActiveChunks};

	ActiveChunks.Reset();

	for (int32 x = ChunkCoordinates.X - ViewDistanceInChunks; x < ChunkCoordinates.X + ViewDistanceInChunks; ++x)
	{
		for (int32 y = ChunkCoordinates.Y - ViewDistanceInChunks; y < ChunkCoordinates.Y + ViewDistanceInChunks; ++y)
		{
			const FVector2D ChunkCoord(x, y);

			if (FChunkData::IsChunkInWorld(ChunkCoord))
			{
				if (ChunkArray[x].ChunkWidthArray[y] == nullptr)
				{
					FIntVector Location(x * FChunkData::ChunkWidthSize, y * FChunkData::ChunkWidthSize, 0.f);

					AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(FVector{Location}, FRotator::ZeroRotator,
					                                               FActorSpawnParameters{});
					checkf(Chunk != nullptr, TEXT("Chunk is not valid."));
					Chunk->InitializeChunk(this, Location);
					ChunkArray[x].ChunkWidthArray[y] = Chunk;
				}
				ChunkArray[x].ChunkWidthArray[y]->SetChunkIsActive(true);
				ActiveChunks.Add(ChunkCoord);
			}

			for (int32 i = 0; i < PreviouslyActiveChunks.Num(); ++i)
			{
				if (PreviouslyActiveChunks[i].Equals(ChunkCoord))
					PreviouslyActiveChunks.RemoveAt(i);
			}
		}
	}
	if (ChunkArray.Num() > 0)
	{
		for (const FVector2D Coord : PreviouslyActiveChunks)
		{
			AChunk* Chunk = ChunkArray[Coord.X].ChunkWidthArray[Coord.Y];
			if (IsValid(Chunk))
				Chunk->SetChunkIsActive(false);
		}
	}
}

void AGameWorld::MovePawnToChunkArrayCenter() const
{
	const FVector SpawnLocation = FVector(FChunkData::WorldSizeInVoxels / 2.f, FChunkData::WorldSizeInVoxels / 2.f,
	                                      FChunkData::ChunkHeightSize + 20);

	checkf(GetWorld()->GetFirstPlayerController()->GetPawn() != nullptr, TEXT("GetPawn is not valid."));
	GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(SpawnLocation);
}

void AGameWorld::GenerateChunks()
{
	for (int32 x = FChunkData::WorldSizeInChunks / 2 - ViewDistanceInChunks; x < FChunkData::WorldSizeInChunks / 2 + ViewDistanceInChunks; ++x)
	{
		for (int32 y = FChunkData::WorldSizeInChunks / 2 - ViewDistanceInChunks; y < FChunkData::WorldSizeInChunks / 2 + ViewDistanceInChunks; ++ y)
		{
			FIntVector Location(x * FChunkData::ChunkWidthSize, y * FChunkData::ChunkWidthSize, 0.f);

			AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(FVector{Location}, FRotator::ZeroRotator, FActorSpawnParameters{});

			checkf(Chunk != nullptr, TEXT("Chunk is not valid."));
			ChunkArray[x].ChunkWidthArray[y] = Chunk;
			ActiveChunks.Add(FVector2D(x, y));

			Chunk->InitializeChunk(this, Location);
		}
	}
}

void AGameWorld::HandleChunkDelayedSpawner(const float DeltaTime)
{
	SpawnChunkDelay -= DeltaTime;

	if (SpawnChunkDelay <= 0.f || ChunksSpawnedCounter <= NumberOfChunksAllowed)
	{
		FMeshData MeshData;
		if (ChunkSpawnerQueue.Dequeue(MeshData))
		{
			if (MeshData.XCoord < ChunkArray.Num() && MeshData.YCoord < ChunkArray[0].ChunkWidthArray.Num())
			{
				AChunk* Chunk = ChunkArray[MeshData.XCoord].ChunkWidthArray[MeshData.YCoord];
				Chunk->SetVoxelMap(MeshData.VoxelMap);
				Chunk->RenderChunk(MeshData.Vertices, MeshData.Triangles, MeshData.UVs);
				Chunk->SetVoxelMapPopulated(true);
				ChunksSpawnedCounter++;

				if (ChunksSpawnedCounter >= NumberOfChunksAllowed - 1 && UGameplayStatics::IsGamePaused(GetWorld()))
					UGameplayStatics::SetGamePaused(GetWorld(), false);
			}
		}
		SpawnChunkDelay = 0.05f;
	}
}

void AGameWorld::UpdatePlayerPositionAndViewDistance()
{
	PlayerPositionChunk = FChunkData::GetChunkPositionFromVector(
		GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());

	if (!PlayerPreviousPositionChunk.Equals(PlayerPositionChunk))
	{
		CheckViewDistance();
		PlayerPreviousPositionChunk = FChunkData::GetChunkPositionFromVector(
			GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	}
}

void AGameWorld::RemovePreviousChunks()
{
	ChunksSpawnedCounter = 0;
	if (ChunkArray.Num() > 0 && ActiveChunks.Num() > 0)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChunk::StaticClass(), FoundActors);

		for (AActor* FoundActor : FoundActors)
			FoundActor->Destroy();

		for (int i = 0; i < ChunkArray.Num(); ++i)
			ChunkArray[i].ChunkWidthArray.Empty();
		ChunkArray.Empty();
	}
}

void AGameWorld::SetCustomSeed(const int64 InCustomSeed)
{
	if (InCustomSeed > 0)
	{
		RandomStream.Initialize(InCustomSeed);
		FMath::RandInit(InCustomSeed);
		CustomSeed = InCustomSeed;
	}
	else
	{
		CustomSeed = FMath::RandRange(1, 800000);
		FMath::RandInit(CustomSeed);
		RandomStream.Initialize(CustomSeed);
	}

	SeededPerlinNoiseNumber = RandomStream.RandRange(0, 8000);
}

void AGameWorld::SetChunkArray(const TArray<FChunkDetails>& InChunkArrayDetails)
{
	RemovePreviousChunks();
	InitializeChunkArray();

	for (int i = 0; i < InChunkArrayDetails.Num(); ++i)
	{
		FChunkDetails ChunkDetails = InChunkArrayDetails[i];
		const FIntVector Location = ChunkDetails.Coordinates * FChunkData::ChunkWidthSize;

		AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(FVector{Location}, FRotator::ZeroRotator, FActorSpawnParameters{});
		Chunk->SetVoxelMapModifications(ChunkDetails.VoxelMapModifications);
		ChunkArray[ChunkDetails.Coordinates.X].ChunkWidthArray[ChunkDetails.Coordinates.Y] = Chunk;
		ActiveChunks.Add(FVector2D(ChunkDetails.Coordinates.X, ChunkDetails.Coordinates.Y));

		Chunk->InitializeChunk(this, Location);
	}
	SetTickableWhenPaused(true);
}

bool AGameWorld::VoxelExistsAndIsSolid(const FVector& Position)
{
	const FVector2D ThisChunkPosition(FMath::FloorToInt(Position.X) / FChunkData::ChunkWidthSize,
	                                  FMath::FloorToInt(Position.Y) / FChunkData::ChunkWidthSize);

	if (!FChunkData::IsChunkInWorld(ThisChunkPosition) || Position.Z < 0 || Position.Z > FChunkData::ChunkHeightSize)
		return false;

	if (ChunkArray[ThisChunkPosition.X].ChunkWidthArray[ThisChunkPosition.Y] != nullptr
		&& ChunkArray[ThisChunkPosition.X].ChunkWidthArray[ThisChunkPosition.Y]->GetVoxelMapPopulated())
		return FVoxelData::VoxelTypes[static_cast<int32>(ChunkArray[ThisChunkPosition.X].ChunkWidthArray[ThisChunkPosition.Y]->GetVoxelFromGlobalFVector(Position).BlockType)]
			.bIsSolid;

	FVoxel Voxel;
	Voxel.InitializeVoxel(Position);
	return FVoxelData::VoxelTypes[static_cast<int32>(Voxel.BlockType)].bIsSolid;
}

AChunk* AGameWorld::GetChunkFromWorldPosition(const FVector& Position)
{
	const FVector2D Coordinates = FChunkData::GetChunkPositionFromVector(Position);
	return ChunkArray[Coordinates.X].ChunkWidthArray[Coordinates.Y];
}
