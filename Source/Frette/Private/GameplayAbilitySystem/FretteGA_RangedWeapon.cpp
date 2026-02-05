#include "GameplayAbilitySystem/FretteGA_RangedWeapon.h"

#include "Weapons/FretteRangedWeaponInstance.h"

void UFretteGA_RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

bool UFretteGA_RangedWeapon::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
		return false;

	const UFretteRangedWeaponInstance* WeaponInstance = Cast<UFretteRangedWeaponInstance>(GetSourceObject(Handle, ActorInfo));
	check(WeaponInstance);

	return WeaponInstance->CurrentAmmo > 0;
}

void UFretteGA_RangedWeapon::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	UFretteRangedWeaponInstance* WeaponInstance = Cast<UFretteRangedWeaponInstance>(GetSourceObject(Handle, ActorInfo));
	check(WeaponInstance);

	WeaponInstance->CurrentAmmo--;
}