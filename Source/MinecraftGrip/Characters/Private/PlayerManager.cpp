#include "MinecraftGrip/Characters/Public/PlayerManager.h"
#include "MinecraftGrip/Characters/Public/BlockPlacer.h"
#include "MinecraftGrip/Minecraft/Public/MinecraftMode.h"
#include "MinecraftGrip/TerrainGeneration/Public/Chunk.h"
#include "MinecraftGrip/TerrainGeneration/Public/GameWorld.h"
#include "MinecraftGrip/TerrainGeneration/Public/VoxelData.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/Material.h"

APlayerManager::APlayerManager() :
	SelectedBlockIndex(1),
	CheckIncrement(100.f),
	ReachableVoxelDistance(800.f)
{
	PrimaryActorTick.bCanEverTick = true;

	SetupPlayerCamera();
	SetupPlayerMovement();
	SetupPlayerVisuals();
}

void APlayerManager::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AGameWorld> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
		GameWorld = *ActorIterator;

	HighlightBlock = GetWorld()->SpawnActor<ABlockPlacer>(FVector{}, FRotator::ZeroRotator);
	HighlightBlock->SetActorHiddenInGame(true);
	HighlightBlock->SetActorScale3D(FVector{1.1f, 1.1f, 1.1f});
	PlaceBlock = GetWorld()->SpawnActor<ABlockPlacer>(FVector{}, FRotator::ZeroRotator);
	PlaceBlock->SetActorHiddenInGame(true);

	GameMode = static_cast<AMinecraftMode*>(AActor::GetWorld()->GetAuthGameMode());
}

void APlayerManager::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlaceCursorBlock();

	if (bDigging)
		DestroyVoxel(DeltaTime);
}

void APlayerManager::SetupPlayerMovement() const
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);
	GetCharacterMovement()->JumpZVelocity = 600;
	GetCharacterMovement()->AirControl = 0.4;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->GravityScale = 1.6;
	GetCharacterMovement()->MaxStepHeight = 0.f;
	GetCharacterMovement()->SetWalkableFloorAngle(0.f);
}

void APlayerManager::SetupPlayerCamera()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 50.f), false, nullptr);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(SpringArm);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetRelativeLocation(FVector{12.f, 0.f, 20.f}, false);
	FirstPersonCamera->FieldOfView = 100.f;
}

void APlayerManager::SetupPlayerVisuals() const
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(TEXT("SkeletalMesh'/Game/_Game/Prefabs/Steve/Models/Steve_Rigged.Steve_Rigged'"));
	checkf(SkeletalMesh.Object != nullptr, TEXT("SkeletalMesh Object is not valid."));
	GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/_Game/Prefabs/SteveMaterial.SteveMaterial'"));
	checkf(Material.Object != nullptr, TEXT("Material Object is not valid."));
	GetMesh()->SetMaterial(0, Material.Object);

	const ConstructorHelpers::FObjectFinder<UClass> Animation(TEXT("Class'/Game/_Game/Animations/AnimationBP.AnimationBP_C'"));
	checkf(Animation.Object, TEXT("Animation Object is not valid."));
	GetMesh()->SetAnimInstanceClass(Animation.Object);

	GetMesh()->SetWorldRotation(FRotator{0, -90, 0}, false, nullptr);
	GetMesh()->SetWorldScale3D(FVector(0.17f, 0.17f, 0.17f));
}

void APlayerManager::HandleMoveForward(const float Value)
{
	checkf(Controller != nullptr, TEXT("Controller is not valid."));
	if (Value != 0)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};

		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};

		AddMovementInput(Direction, Value);
	}
}

void APlayerManager::HandleMoveRight(const float Value)
{
	checkf(Controller != nullptr, TEXT("Controller is not valid."));
	if (Value != 0)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};

		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};

		AddMovementInput(Direction, Value);
	}
}

void APlayerManager::ToggleThirdPerson()
{
	SpringArm->TargetArmLength > 0.f ? SpringArm->TargetArmLength = 0.f : SpringArm->TargetArmLength = 500.f;
}

void APlayerManager::HandleStartDigging()
{
	if (CanManipulateBlocks)
		bDigging = true;
}

void APlayerManager::HandleStopDigging()
{
	bDigging = false;
	DiggingStarted = false;
}

void APlayerManager::HandleStartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetMesh()->GetAnimInstance()->Montage_SetPlayRate(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage(), 3.f);
}

void APlayerManager::HandleStopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetMesh()->GetAnimInstance()->Montage_SetPlayRate(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage(), 2.f);
}

void APlayerManager::HandlePlaceBlock()
{
	if (CanManipulateBlocks)
		GameWorld->GetChunkFromWorldPosition(PlaceBlock->GetActorLocation())->UpdateChunkWithVoxel(PlaceBlock->GetActorLocation(), FVoxelData::VoxelTypes[SelectedBlockIndex].BlockType);
}

void APlayerManager::PlaceCursorBlock()
{
	float Step = CheckIncrement;
	FVector LastPosition{};

	while (Step < ReachableVoxelDistance)
	{
		FVector Location;
		FRotator Rotator;
		GetController()->GetPlayerViewPoint(Location, Rotator);


		FVector TargetPosition = Location + UKismetMathLibrary::GetForwardVector(Rotator) * Step;
		if (GameWorld->VoxelExistsAndIsSolid(TargetPosition))
		{
			CanManipulateBlocks = true;
			HighlightBlock->SetActorLocation(FVector(AChunk::FVectorToFlooredVector(TargetPosition)) + 50.f);
			PlaceBlock->SetActorLocation(LastPosition);

			HighlightBlock->SetActorHiddenInGame(false);

			return;
		}

		LastPosition = FVector(AChunk::FVectorToFlooredVector(TargetPosition));

		Step += CheckIncrement;
		CanManipulateBlocks = false;
	}

	HighlightBlock->SetActorHiddenInGame(true);
	CanManipulateBlocks = false;
}

void APlayerManager::DestroyVoxel(const float DeltaTime)
{
	if (!DiggingStarted)
	{
		DiggedVoxelPosition = HighlightBlock->GetActorLocation();
		DiggedVoxel = GameWorld->GetChunkFromWorldPosition(DiggedVoxelPosition)->GetVoxelFromGlobalFVector(DiggedVoxelPosition);
		TimeToDestroyVoxel = DiggedVoxel.DestroyTime;
		DiggingStarted = true;
	}
	else
	{
		if (TimeToDestroyVoxel > 0)
			TimeToDestroyVoxel -= DeltaTime;
		else
		{
			GameWorld->GetChunkFromWorldPosition(DiggedVoxelPosition)->UpdateChunkWithVoxel(DiggedVoxelPosition, EBlockType::AirBlock);
			DiggingStarted = false;
		}
	}
}

void APlayerManager::HandleMouseScrollUp()
{
	SelectedBlockIndex++;

	if (SelectedBlockIndex > FVoxelData::VoxelTypes.Num() - 1)
		SelectedBlockIndex = 1;
}

void APlayerManager::HandleMouseScrollDown()
{
	SelectedBlockIndex--;

	if (SelectedBlockIndex < 1)
		SelectedBlockIndex = FVoxelData::VoxelTypes.Num() - 1;
}

void APlayerManager::HandleSaveGame()
{
	GameMode->HandleSaveGame(GameWorld);
}

void APlayerManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	checkf(PlayerInputComponent != nullptr, TEXT("PlayerInputComponent is not valid."));

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerManager::HandleMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerManager::HandleMoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerManager::HandleStartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerManager::HandleStopSprinting);

	PlayerInputComponent->BindAction("ToggleThirdPerson", IE_Pressed, this, &APlayerManager::ToggleThirdPerson);

	PlayerInputComponent->BindAction("Dig", IE_Pressed, this, &APlayerManager::HandleStartDigging);
	PlayerInputComponent->BindAction("Dig", IE_Released, this, &APlayerManager::HandleStopDigging);

	PlayerInputComponent->BindAction("PlaceBlock", IE_Pressed, this, &APlayerManager::HandlePlaceBlock);

	PlayerInputComponent->BindAction("MouseScrollUp", IE_Pressed, this, &APlayerManager::HandleMouseScrollUp);
	PlayerInputComponent->BindAction("MouseScrollDown", IE_Pressed, this, &APlayerManager::HandleMouseScrollDown);

	PlayerInputComponent->BindAction("SaveGame", IE_Pressed, this, &APlayerManager::HandleSaveGame);
}
