#include "MinecraftGrip/Characters/Public/BlockPlacer.h"

ABlockPlacer::ABlockPlacer()
{
	PrimaryActorTick.bCanEverTick = false;
	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	CubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	checkf(CubeMesh != nullptr, TEXT("CubeMesh is not valid."));
	CubeMeshComponent->SetStaticMesh(CubeMesh);

	CubeMeshComponent->SetMaterial(
		0, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/_Game/Materials/PlaceBlockMaterial.PlaceBlockMaterial'")).Object);
	RootComponent = CubeMeshComponent;
	SetActorEnableCollision(false);
}

void ABlockPlacer::BeginPlay()
{
	Super::BeginPlay();
}

void ABlockPlacer::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}
