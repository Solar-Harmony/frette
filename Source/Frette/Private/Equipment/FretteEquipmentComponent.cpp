#include "Equipment/FretteEquipmentComponent.h"

#include "Frette/Frette.h"
#include "Inventory/Items/FretteGearItem.h"
#include "Player/FrettePlayerState.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_EquipmentSlot_Head, "Frette.EquipmentSlot.Head");
UE_DEFINE_GAMEPLAY_TAG(TAG_EquipmentSlot_Chest, "Frette.EquipmentSlot.Chest");
UE_DEFINE_GAMEPLAY_TAG(TAG_EquipmentSlot_Legs, "Frette.EquipmentSlot.Legs");
UE_DEFINE_GAMEPLAY_TAG(TAG_EquipmentSlot_Feet, "Frette.EquipmentSlot.Feet");
UE_DEFINE_GAMEPLAY_TAG(TAG_EquipmentSlot_HandLeft, "Frette.EquipmentSlot.HandLeft");
UE_DEFINE_GAMEPLAY_TAG(TAG_EquipmentSlot_HandRight, "Frette.EquipmentSlot.HandRight");

void UFretteEquipmentComponent::Initialize(const AFrettePlayerState* PlayerState)
{
	PlayerState->GetPlayerInventory()->OnItemSelected.AddUObject(this, &UFretteEquipmentComponent::OnInventoryItemSelected);
}

void UFretteEquipmentComponent::OnInventoryItemSelected(const UFretteInventoryItem* Item)
{
	if (Item->IsA<UFretteGearItem>())
	{
		UE_LOG(LogFrette, Log, TEXT("Equipment: Received new item '%s'."), *Item->Data->DisplayName.ToString());
	}
}