#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlockPlacer.generated.h"

/**
 * Class used for creating a transparent block that can be placed on the map to visualize what block will be digged.
 */
UCLASS()
class MINECRAFTGRIP_API ABlockPlacer final : public AActor
{
	GENERATED_BODY()

public:
	ABlockPlacer();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UStaticMeshComponent* CubeMeshComponent;
	UPROPERTY()
	UStaticMesh* CubeMesh;

};
