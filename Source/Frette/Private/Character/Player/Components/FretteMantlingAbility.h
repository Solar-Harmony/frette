#pragma once

#include "CoreMinimal.h"
#include "Character/FretteBaseCharacter.h"
#include "Character/Components/AbilitySystem/FretteGameplayAbility.h"
#include "FretteMantlingAbility.generated.h"

UCLASS()
class UFretteMantlingAbility : public UFretteGameplayAbility
{
	GENERATED_BODY()

public:
	UFretteMantlingAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(Units="cm", ClampMin=0))
	float MaxPlayerToWallDistance = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, meta=(Units="cm", ClampMin=0))
	float MaxWallToLedgeDistance = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, meta=(Units="cm", ClampMin=0))
	float MinObstacleHeight = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, meta=(Units="cm", ClampMin=0))
	float MaxObstacleHeight = 150.0f;

	UPROPERTY(EditDefaultsOnly, meta=(Units="cm/s", ClampMin=10))
	float BaseClimbSpeed = 160.0f;

	UPROPERTY(EditDefaultsOnly, meta=(Units="s", ClampMin=0.1))
	float MinDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, meta=(Units="s", ClampMin=0.1))
	float MaxDuration = 1.0f;

	UFUNCTION()
	void OnMoveCompleted();

private:
	bool TryMantling(AFretteBaseCharacter* Player);
	
	bool HasEnoughSpaceAbove(const AFretteBaseCharacter* Player) const;
	TOptional<FHitResult> DetectWall(const AFretteBaseCharacter* Player) const;
	TOptional<FHitResult> DetectLedge(const AFretteBaseCharacter* Player, const FHitResult& Wall) const;
	bool FitsInSpace(const AFretteBaseCharacter* Player, const FVector& Location) const;
	bool PerformMantling(AFretteBaseCharacter* Player, TOptional<FHitResult> Ledge, FVector TargetLocation, UPrimitiveComponent* LedgeComp);
};