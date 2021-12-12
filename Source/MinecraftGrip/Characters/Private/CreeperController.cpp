#include "MinecraftGrip/Characters/Public/CreeperController.h"
#include "MinecraftGrip/TerrainGeneration/Public/Chunk.h"
#include "MinecraftGrip/TerrainGeneration/Public/ChunkData.h"
#include "MinecraftGrip/TerrainGeneration/Public/GameWorld.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Perception/PawnSensingComponent.h"

ACreeperController::ACreeperController():
	TimeUntilExplosion(2.5f),
	TimeUntilMaterialSwap(0.2f),
	ExplosionStrength(200.f)
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	SetupCreeperAudio();
	SetupCreeperVisuals();
	SetupCreeperMovement();
}

void ACreeperController::BeginPlay()
{
	Super::BeginPlay();

	SetupAIMovement();

	GameWorld = Cast<AGameWorld>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameWorld::StaticClass()));

	GetMesh()->SetWorldScale3D(FVector{7.f, 7.f, 7.f});
	GetMesh()->SetWorldRotation(FRotator{0.f, -90.f, 0.f});
}

void ACreeperController::HandleNavMeshUpdate()
{
	CurrentChunkPosition = FChunkData::GetChunkPositionFromVector(GetActorLocation());

	if (CurrentChunkPosition != PreviousChunkPosition)
	{
		NavigationSystem->OnNavigationBoundsUpdated(NavMeshBoundsVolume);
		PreviousChunkPosition = CurrentChunkPosition;
	}
}

void ACreeperController::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsExploding)
		HandleCreeperTicking(DeltaTime);

	MoveToRandomLocation();
	HandleNavMeshUpdate();
}

void ACreeperController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACreeperController::SetupCreeperVisuals()
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(TEXT("SkeletalMesh'/Game/_Game/Prefabs/CreeperAI/source/creeper.creeper'"));
	checkf(SkeletalMesh.Object != nullptr, TEXT("SkeletalMesh Object is not valid."));
	GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/_Game/Prefabs/CreeperAI/textures/creeper_Mat.creeper_Mat'"));
	checkf(Material.Object != nullptr, TEXT("Material Object is not valid."));
	CreeperMaterial = Material.Object;

	const ConstructorHelpers::FObjectFinder<UMaterial> WhiteMat(TEXT("Material'/Game/_Game/Prefabs/CreeperAI/source/02_-_Default.02_-_Default'"));
	checkf(WhiteMat.Object != nullptr, TEXT("Material Object is not valid."));
	WhiteMaterial = WhiteMat.Object;
}

void ACreeperController::SetupCreeperMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxStepHeight = 120.f;
}

void ACreeperController::SetupAIMovement()
{
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ACreeperController::MoveToPlayer);
	PawnSensingComponent->SetPeripheralVisionAngle(180.f);
	PawnSensingComponent->SightRadius = 5000.f;
	NavMeshBoundsVolume = Cast<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass()));

	NavMeshBoundsVolume->AttachToActor(this, FAttachmentTransformRules{EAttachmentRule::KeepWorld, true});

	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	APawn::SpawnDefaultController();
	AIController = Cast<AAIController>(GetController());

	AIController->ReceiveMoveCompleted.AddDynamic(this, &ACreeperController::OnMoveToPlayerCompleted);
}

void ACreeperController::SetupCreeperAudio()
{
	static ConstructorHelpers::FObjectFinder<USoundWave> ExplosionSound(TEXT("SoundWave'/Game/_Game/Sounds/Creeper_Explosion_-_Sound_Effect.Creeper_Explosion_-_Sound_Effect'"));
	ExplosionSoundWave = ExplosionSound.Object;
}

void ACreeperController::HandleMaterialSwap()
{
	MaterialToBeSwapped = MaterialToBeSwapped == WhiteMaterial ? CreeperMaterial : WhiteMaterial;
	GetMesh()->SetMaterial(0, MaterialToBeSwapped);
	TimeUntilMaterialSwap = 0.2f;
}

void ACreeperController::HandleCreeperSizeIncrease()
{
	const float LerpScale = FMath::Lerp(GetActorScale3D().X, 1.5f, 0.01f);
	SetActorScale3D(FVector{LerpScale, LerpScale, LerpScale});
}

void ACreeperController::MoveToPlayer(APawn* InPawn)
{
	IsMovingRandomly = false;

	if (IsMovingToPawn)
		return;

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	checkf(GetWorld() != nullptr, TEXT("GetWorld is not valid."));
	FAIMoveRequest MoveRequest{UGameplayStatics::GetPlayerPawn(GetWorld(), 0)};
	MoveRequest.SetAcceptanceRadius(300.f);

	checkf(AIController != nullptr, TEXT("AIController is not valid."));
	const FPathFollowingRequestResult Result = AIController->MoveTo(MoveRequest);

	if (Result.Code.GetValue() == EPathFollowingRequestResult::RequestSuccessful)
		IsMovingToPawn = true;
}

void ACreeperController::MoveToRandomLocation()
{
	if (IsMovingRandomly || IsMovingToPawn)
		return;

	GetCharacterMovement()->MaxWalkSpeed = 100.f;
	FNavLocation LocationToMove;
	NavigationSystem->GetRandomReachablePointInRadius(GetActorLocation(), 5000.f, LocationToMove);

	const EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(LocationToMove);

	if (Result == EPathFollowingRequestResult::RequestSuccessful)
		IsMovingRandomly = true;
}

void ACreeperController::HandleCreeperExplosion()
{
	AChunk* Chunk = GameWorld->GetChunkFromWorldPosition(GetActorLocation());
	Chunk->MakeExplosionAt(GetActorLocation(), 2000);
	IsMovingToPawn = false;

	ExplosionStrength = 200.f;
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetCharacterMovement()->AddImpulse(
		(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation() - GetActorLocation()) * ExplosionStrength, false);

	TArray<AActor*> ChildrenActors;
	GetAttachedActors(ChildrenActors);

	for (AActor* ChildrenActor : ChildrenActors)
		ChildrenActor->Destroy();

	Destroy();
}

void ACreeperController::HandleCreeperTicking(const float DeltaTime)
{
	TimeUntilExplosion -= DeltaTime;
	TimeUntilMaterialSwap -= DeltaTime;

	HandleCreeperSizeIncrease();

	if (TimeUntilMaterialSwap <= 0)
		HandleMaterialSwap();

	if (TimeUntilExplosion <= 0)
		HandleCreeperExplosion();
}

void ACreeperController::OnMoveToPlayerCompleted(FAIRequestID InID, const EPathFollowingResult::Type InResult)
{
	if (InResult == EPathFollowingResult::Type::Aborted && (IsMovingRandomly || IsMovingToPawn))
	{
		IsMovingToPawn = false;
		IsMovingRandomly = false;
	}
	else if (InResult == EPathFollowingResult::Type::Success && IsMovingToPawn)
	{
		IsExploding = true;
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSoundWave, GetActorLocation());
	}
	else if (InResult == EPathFollowingResult::Type::Success && IsMovingRandomly)
	{
		IsMovingRandomly = false;
		MoveToRandomLocation();
	}
}
