#include "GameplayAbilitySystem/FretteGA_RangedWeapon.h"

#include "Character/FretteBaseCharacter.h"
#include "Character/FretteNotificationsComponent.h"
#include "Character/FrettePlayerCharacter.h"
#include "Frette/Frette.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Weapons/FretteProjectile.h"

void UFretteGA_RangedWeapon::SpawnProjectile(const UFretteRangedWeaponItem* WeaponInstance)
{
	const AActor* WeaponActor = WeaponInstance->SpawnedActor;

	const FTransform WeaponWorldTransform = WeaponActor->GetActorTransform();
	const FTransform ProjectileSpawnTransform = WeaponInstance->GetData()->ProjectileSpawnTransform;
	const FTransform SpawnWorldTransform = ProjectileSpawnTransform * WeaponWorldTransform;
	const FVector MuzzleLocation = SpawnWorldTransform.GetLocation();
	const APawn* InstigatorPawn = WeaponInstance->GetOwningPlayer();

	const AController* Controller = InstigatorPawn ? InstigatorPawn->GetController() : nullptr;

	ensure(Controller);

	FRotator SpawnRotation = WeaponInstance->GetOwningPlayer()->GetControlRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = WeaponInstance->GetOwningPlayer();
	SpawnParams.Owner = SpawnParams.Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AFretteProjectile>(
		WeaponInstance->GetData()->ProjectileType,
		MuzzleLocation,
		SpawnRotation,
		SpawnParams
		);

}

UFretteRangedWeaponItem* UFretteGA_RangedWeapon::GetWeaponInstance() const
{
	return Cast<UFretteRangedWeaponItem>(GetCurrentSourceObject());
}

bool UFretteGA_RangedWeapon::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
		return false;

	const UFretteRangedWeaponItem* WeaponInstance = GetWeaponInstance();
	if (!ensure(WeaponInstance))
		return false;

	const bool bEnoughAmmo = WeaponInstance->GetCurrentAmmo() > 0;
	if (!bEnoughAmmo)
	{
		FRETTE_LOG(Log, "%s tried to use %s but didn't have enough ammo!", *ActorInfo->OwnerActor->GetName(), *WeaponInstance->GetData()->GetName());
		const AFrettePlayerCharacter* Zouave = Cast<AFrettePlayerCharacter>(GetAvatarActor());
		if (Zouave != nullptr)
		{
			UFretteNotificationsComponent::Notify(Zouave, INVTEXT("You are out of ammo. Press R to reload!"));	
		}
	}
	
	return bEnoughAmmo;
}

void UFretteGA_RangedWeapon::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	UFretteRangedWeaponItem* WeaponInstance = GetWeaponInstance();
	check(WeaponInstance);

	WeaponInstance->TryUseAmmo();
	
	FRETTE_LOG(Log, "%s's %s now has %d ammo left", *ActorInfo->OwnerActor->GetName(), *WeaponInstance->GetData()->GetName(), WeaponInstance->GetCurrentAmmo());
}