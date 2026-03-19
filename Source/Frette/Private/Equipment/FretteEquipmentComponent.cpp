#include "Equipment/FretteEquipmentComponent.h"
#include "Inventory/Items/FretteSlottableItem.h"

// TODO: Enlever dans le merge, Yanick va les definir aussi
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Head,	"Frette.BodyPart.Head");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Torso,	"Frette.BodyPart.Torso");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Legs,	"Frette.BodyPart.Legs");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Feet,	"Frette.BodyPart.Feet");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Hands,	"Frette.BodyPart.Hands");

// live ça fait evidemment pas grand chose mais on devrait s'occuper d'afficher l'item visuel là maybe
void UFretteEquipmentComponent::EquipItem_Implementation(UFretteSlottableItem* Item) const
{
	Item->OnEquipped();
}

void UFretteEquipmentComponent::UnequipItem_Implementation(UFretteSlottableItem* Item) const
{
	Item->OnUnequipped();
}