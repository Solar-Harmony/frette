#pragma once

#include "Inventory/Items/FretteInventoryItem.h"
#include "Inventory/FretteInventoryComponent.h"

UFretteInventoryComponent* UFretteInventoryItem::GetOwningInventory() const
{
	return Cast<UFretteInventoryComponent>(GetOuter());
}