#include "Inventory/Items/Impl/Weapons/FretteRangedWeaponItem.h"
#include "Net/UnrealNetwork.h"

void UFretteRangedWeaponItem::UseAmmo()
{
	CurrentClipAmmo -= 1;
}

void UFretteRangedWeaponItem::Reload()
{
	UFretteRangedWeaponDataAsset* ItemData = Cast<UFretteRangedWeaponDataAsset>(Data);
	CurrentClipAmmo = ItemData->MaxClipAmmo;
}
