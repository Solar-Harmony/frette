#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/FretteGameplayAbility.h"
#include "Character/FretteBaseCharacter.h"
#include "FretteMantlingAbility.generated.h"

UCLASS()
class UFretteMantlingAbility : public UFretteGameplayAbility
{
	GENERATED_BODY()

public:
	UFretteMantlingAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Mantling")
	float MaxDistanceToWall = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mantling")
	float MaxLedgeGrabDistance = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mantling")
	float MaxObstacleHeight = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Mantling", meta=(Units="cm/s"))
	float BaseSpeed = 160.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Mantling")
	float MinDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Mantling")
	float MaxDuration = 1.0f;

	UFUNCTION()
	void OnMoveCompleted();

private:
	bool TryMantling(AFretteBaseCharacter* Player);
	TOptional<FHitResult> DetectWall(AFretteBaseCharacter* Player) const;
	TOptional<FHitResult> DetectLedge(AFretteBaseCharacter* Player, const FHitResult& Wall) const;
	bool HasEnoughSpaceAbove(const AFretteBaseCharacter* Player) const;
	bool FitsInSpace(const AFretteBaseCharacter* Player, const FVector& Location) const;
};