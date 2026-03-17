#include "GameplayAbilitySystem/FretteGA_RangedWeapon.h"

#include "Character/FretteBaseCharacter.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Weapons/FretteProjectile.h"

//TODO: Faire que tout les équipements ont un Use ability qui peut être associer 
//
void UFretteGA_RangedWeapon::SpawnProjectile(const UFretteRangedWeaponItem* WeaponInstance)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	//@TODO: Devrait utiliser un socket/muzzle location plutôt que le PawnViewLocation
	FVector MuzzleLocation = Character->GetPawnViewLocation();

	//Utilise le controller.Rotation pour les IA ce qui n'est peut-être pas ce que l'on veux
	FRotator AimRotation = Character->GetBaseAimRotation();

	MuzzleLocation += AimRotation.Vector() * 100.f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = WeaponInstance->GetOwningPlayer();
	SpawnParams.Owner = SpawnParams.Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AFretteProjectile>(
		WeaponInstance->GetData()->ProjectileType,
		MuzzleLocation,
		AimRotation,
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
	if (!WeaponInstance)
		return false;

	return WeaponInstance->GetCurrentAmmo() > 0;
}

void UFretteGA_RangedWeapon::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	UFretteRangedWeaponItem* WeaponInstance = GetWeaponInstance();
	check(WeaponInstance);

	WeaponInstance->UseAmmo();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,TEXT("Weapon current ammo: ") + FString::FromInt(WeaponInstance->GetCurrentAmmo()));

}