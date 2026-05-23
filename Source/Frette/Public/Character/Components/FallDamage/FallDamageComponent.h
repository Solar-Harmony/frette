#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Character/Components/FallDamage/FallDamageDataAsset.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FallDamageComponent.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Movement_FallDamage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFallDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	void BindMovementEvents();
	void ApplyFallDamage(float DistanceFallen) const;

	UFUNCTION()
	void MovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	UFUNCTION()
	void Landed(const FHitResult& Hit);

	UFUNCTION()
	void OnJumpApexReached();
	int32 FindWorstDamageThresholdIndex(float DistanceFallen) const;
	float CalculateFinalDamage(float DistanceFallen, int32 WorstThresholdIndex, const FFallDamageThreshold& Threshold) const;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	float FallStartHeight = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TObjectPtr<UFallDamageDataAsset> Config;
};