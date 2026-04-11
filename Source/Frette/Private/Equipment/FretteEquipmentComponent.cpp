#include "Equipment/FretteEquipmentComponent.h"
#include "Inventory/Items/FretteSlottableItem.h"

// live ça fait evidemment pas grand chose mais on devrait s'occuper d'afficher l'item visuel là maybe
void UFretteEquipmentComponent::EquipItem_Implementation(UFretteSlottableItem* Item) const
{
	Item->OnEquipped();
	Client_OnItemEquipped(Item);
}

void UFretteEquipmentComponent::UnequipItem_Implementation(UFretteSlottableItem* Item) const
{
	Item->OnUnequipped();
	Client_OnItemUnequipped(Item);
}

void UFretteEquipmentComponent::Client_OnItemEquipped_Implementation(UFretteSlottableItem* Item) const
{
	Item->OnEquipped();
}

void UFretteEquipmentComponent::Client_OnItemUnequipped_Implementation(UFretteSlottableItem* Item) const
{
	Item->OnUnequipped();
}