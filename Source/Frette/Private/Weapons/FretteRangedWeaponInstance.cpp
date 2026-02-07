#include "Weapons/FretteRangedWeaponInstance.h"

#include "Net/UnrealNetwork.h"

UFretteRangedWeaponInstance::UFretteRangedWeaponInstance()
{

	CurrentClipAmmo = MaxClipAmmo;
}

void UFretteRangedWeaponInstance::UseAmmo()
{
	CurrentClipAmmo -= 1;
}

void UFretteRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFretteRangedWeaponInstance, CurrentClipAmmo, COND_OwnerOnly);

}

void UFretteRangedWeaponInstance::OnRep_ClipAmmo(int OldPrimaryClipAmmo) {}