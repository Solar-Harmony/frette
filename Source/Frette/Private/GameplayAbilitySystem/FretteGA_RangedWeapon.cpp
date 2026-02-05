#include "GameplayAbilitySystem/FretteGA_RangedWeapon.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Weapons/FretteProjectile.h"
#include "Weapons/FretteRangedWeaponInstance.h"

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

void UFretteGA_RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,TEXT("Ranged Weapon Ability Activated"));
	UFretteRangedWeaponInstance* WeaponInstance = Cast<UFretteRangedWeaponInstance>(GetSourceObject(Handle, ActorInfo));
	check(WeaponInstance);

	SpawnProjectile(WeaponInstance);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,TEXT("Weapon current ammo: ") + FString::FromInt(WeaponInstance->CurrentAmmo));

}