#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "MinecraftMode.generated.h"

class AGameWorld;
class USaveGame;

/**
 * Default mode for Minecraft, handles saving and loading the game.
 */

UCLASS()
class MINECRAFTGRIP_API AMinecraftMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMinecraftMode();

	// Saves the current game world.
	UFUNCTION(BlueprintCallable)
	void HandleSaveGame(const AGameWorld* InGameWorld);

	// Loads back the latest saved game and restarts the game.
	UFUNCTION(BlueprintCallable)
	void HandleLoadGame(AGameWorld* InGameWorld);
	
};
