#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/FretteGameplayAbility.h"
#include "Inventory/Items/Impl/Weapons/FretteRangedWeaponItem.h"
#include "FretteGA_RangedWeapon.generated.h"

UCLASS()
class UFretteGA_RangedWeapon : public UFretteGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const UFretteRangedWeaponItem* WeaponInstance);
	FRotator GetAimRotation(const APawn* InstigatorPawn, FVector MuzzleLocation) const;

	UFUNCTION(BlueprintCallable)
	UFretteRangedWeaponItem* GetWeaponInstance() const;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
};