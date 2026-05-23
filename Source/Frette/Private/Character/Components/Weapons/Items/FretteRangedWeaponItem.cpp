#include "Character/Components/Weapons/Items/FretteRangedWeaponItem.h"

#include "Frette.h"
#include "Inventory/FretteInventoryComponent.h"

bool UFretteRangedWeaponItem::TryUseAmmo()
{
	if (NumBulletsLoaded <= 0)
		return false;

	NumBulletsLoaded -= 1;
	return true;
}

int UFretteRangedWeaponItem::Reload()
{
	if (NumBulletsLoaded > 0)
		return -1;
	
	const TSubclassOf<UFretteStackableItemDataAsset> AmmoClass = GetData()->AmmoType.GetClass();
	UFretteStackableItem* AmmoItem = Cast<UFretteStackableItem>(GetOwningInventory()->GetFirstItemFromAsset(AmmoClass));
	if (AmmoItem == nullptr)
		return 0;
	
	const int NumBulletsToLoad = GetData()->MaxClipAmmo;
	const int NumBulletsDelta = AmmoItem->UpdateQuantity(-NumBulletsToLoad);
	NumBulletsLoaded = -NumBulletsDelta;
	
	GetOwningInventory()->ChangeItem(this);
	GetOwningInventory()->ChangeItem(AmmoItem);
	
	return NumBulletsLoaded;
}
