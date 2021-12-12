#include "MinecraftGrip/Characters/Public/PlayerAnimInstance.h"
#include "MinecraftGrip/Characters/Public/PlayerManager.h"

void UPlayerAnimInstance::UpdateAnimationProperties(float const DeltaTime)
{
	if (!Character)
		Character = Cast<APlayerManager>(TryGetPawnOwner());

	if (Character)
	{
		FVector Velocity{Character->GetVelocity()};
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bDigging = Character->GetDigging();
	}
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<APlayerManager>(TryGetPawnOwner());
}
