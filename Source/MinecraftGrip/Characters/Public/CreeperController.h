#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"

#include "CreeperController.generated.h"

class AAIController;
struct FAIRequestID;
class AGameWorld;
class UNavigationSystemV1;
class ANavMeshBoundsVolume;
class UPawnSensingComponent;

/**
 * Controller class that is used for Creeper NPC type that explodes on contact with player, driven by AI
 */
UCLASS()
class MINECRAFTGRIP_API ACreeperController final : public ACharacter
{
	GENERATED_BODY()

public:
	ACreeperController();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

private: // Setup of creeper visuals and AI
	void SetupCreeperVisuals();
	void SetupCreeperMovement();
	void SetupAIMovement();
	void SetupCreeperAudio();
	void HandleMaterialSwap();
	void HandleCreeperSizeIncrease();

private: // AI controlling methods
	void HandleNavMeshUpdate();
	void MoveToRandomLocation();
	void HandleCreeperTicking(const float DeltaTime);
	void HandleCreeperExplosion();
	
	UFUNCTION()
	void MoveToPlayer(APawn* InPawn);
	
	UFUNCTION()
	void OnMoveToPlayerCompleted(FAIRequestID InID, EPathFollowingResult::Type InResult);

private: // Attributes for AI movement 
	bool IsMovingToPawn;
	bool IsMovingRandomly;
	FVector2D CurrentChunkPosition;
	FVector2D PreviousChunkPosition;
	UPROPERTY()
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	ANavMeshBoundsVolume* NavMeshBoundsVolume;

	UPROPERTY()
	UNavigationSystemV1* NavigationSystem;

private: // Attributes for visuals.
	bool IsExploding;
	float TimeUntilExplosion;
	float TimeUntilMaterialSwap;
	
	UPROPERTY()
	USoundWave* ExplosionSoundWave;

	UPROPERTY()
	UMaterial* WhiteMaterial;

	UPROPERTY()
	UMaterial* MaterialToBeSwapped;

	UPROPERTY()
	UMaterial* CreeperMaterial;

private: // Attributes needed for functionalities.
	UPROPERTY()
	AGameWorld* GameWorld;

private: // Attributes for creeper statistics.
	float ExplosionStrength;

};
