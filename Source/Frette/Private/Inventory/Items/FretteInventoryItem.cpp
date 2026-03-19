#pragma once

#include "Inventory/Items/FretteInventoryItem.h"

#include "Character/FretteBaseCharacter.h"
#include "Inventory/FretteInventoryComponent.h"

UFretteInventoryComponent* UFretteInventoryItem::GetOwningInventory() const
{
	return Cast<UFretteInventoryComponent>(GetOuter());
}

AFretteBaseCharacter* UFretteInventoryItem::GetOwningPlayer() const
{
	return Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());
}