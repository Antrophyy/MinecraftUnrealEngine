#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "PlayerAnimInstance.generated.h"

class APlayerManager;

/**
 * Handles player animation playback.
 */
UCLASS()
class MINECRAFTGRIP_API UPlayerAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Checks player status and sets animation for it.
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float const DeltaTime);

	virtual void NativeInitializeAnimation() override;

private: // Character properties for animation.
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APlayerManager* Character;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true") )
	bool bDigging;
	
};
