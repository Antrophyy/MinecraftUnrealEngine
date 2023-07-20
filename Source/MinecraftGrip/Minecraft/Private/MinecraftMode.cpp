#include "MinecraftGrip/Minecraft/Public/MinecraftMode.h"
#include "MinecraftGrip/Characters/Public/PlayerManager.h"
#include "MinecraftGrip/Minecraft/Public/SaveMinecraftGame.h"
#include "MinecraftGrip/TerrainGeneration/Public/Chunk.h"
#include "MinecraftGrip/TerrainGeneration/Public/ChunkData.h"
#include "MinecraftGrip/TerrainGeneration/Public/GameWorld.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMinecraftMode::AMinecraftMode()
{
	DefaultPawnClass = APlayerManager::StaticClass();

	static ConstructorHelpers::FObjectFinder<UClass> MainHUD(TEXT("Class'/Game/_Game/GUI/MainHUD_BP.MainHUD_BP_C'"));

	HUDClass = MainHUD.Object;
}

void AMinecraftMode::HandleSaveGame(const AGameWorld* InGameWorld)
{
	if (USaveMinecraftGame* SaveGameInstance = Cast<USaveMinecraftGame>(UGameplayStatics::CreateSaveGameObject(USaveMinecraftGame::StaticClass())))
	{
		TArray<FChunkDetails> GameMap;

		const TArray<FChunkWidth> ChunkArray = InGameWorld->GetChunkArray();

		for (int i = 0; i < ChunkArray.Num(); ++i)
		{
			for (int j = 0; j < ChunkArray[i].ChunkWidthArray.Num(); ++j)
			{
				const AChunk* Chunk = ChunkArray[i].ChunkWidthArray[j];

				if (Chunk == nullptr)
					continue;

				FChunkDetails ChunkDetail;
				ChunkDetail.Coordinates = Chunk->GetSpawnLocation() / FChunkData::ChunkWidthSize;
				ChunkDetail.VoxelMapModifications = Chunk->GetVoxelMapModifications();
				GameMap.Add(ChunkDetail);
			}
		}

		SaveGameInstance->SetGameMap(GameMap);

		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		PlayerLocation.Z += 2000;
		SaveGameInstance->SetPlayerPosition(PlayerLocation);
		SaveGameInstance->SetActiveChunksCoordinates(InGameWorld->GetActiveChunksArray());
		SaveGameInstance->SetCustomSeed(InGameWorld->GetCustomSeed());

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, "MinecraftGameSave5", 0);
	}
}

void AMinecraftMode::HandleLoadGame(AGameWorld* InGameWorld)
{
	if (const USaveMinecraftGame* LoadedGame = Cast<USaveMinecraftGame>(UGameplayStatics::LoadGameFromSlot("MinecraftGameSave5", 0)))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		
		InGameWorld->SetCustomSeed(LoadedGame->GetCustomSeed());
		GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(LoadedGame->GetPlayerPosition());
		InGameWorld->SetActiveChunkArray(LoadedGame->GetActiveChunksCoordinates());
		InGameWorld->SetChunkArray(LoadedGame->GetGameMap());
	}
}
