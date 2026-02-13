#include "GameplayAbilitySystem/FretteGA_RangedWeapon.h"

#include "GameFramework/Character.h"
#include "Weapons/FretteProjectile.h"
#include "Weapons/FretteRangedWeaponInstance.h"

//TODO: Faire que tout les équipements ont un Use ability qui peut être associer 
//
void UFretteGA_RangedWeapon::SpawnProjectile(const UFretteRangedWeaponInstance* WeaponInstance)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	//@TODO: Devrait utiliser un socket/muzzle location plutôt que le PawnViewLocation
	FVector MuzzleLocation = Character->GetPawnViewLocation();

	//Utilise le controller.Rotation pour les IA ce qui n'est peut-être pas ce que l'on veux
	FRotator AimRotation = Character->GetBaseAimRotation();

	MuzzleLocation += AimRotation.Vector() * 100.f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = WeaponInstance->GetPawn();
	SpawnParams.Owner = SpawnParams.Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AFretteProjectile>(
		WeaponInstance->ProjectileType,
		MuzzleLocation,
		AimRotation,
		SpawnParams
		);
}

UFretteRangedWeaponInstance* UFretteGA_RangedWeapon::GetWeaponInstance() const
{
	return Cast<UFretteRangedWeaponInstance>(GetCurrentSourceObject());
}

bool UFretteGA_RangedWeapon::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
		return false;

	const UFretteRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	check(WeaponInstance);

	return WeaponInstance->GetCurrentAmmo() > 0;
}

void UFretteGA_RangedWeapon::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	UFretteRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	check(WeaponInstance);

	WeaponInstance->UseAmmo();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,TEXT("Weapon current ammo: ") + FString::FromInt(WeaponInstance->GetCurrentAmmo()));

}