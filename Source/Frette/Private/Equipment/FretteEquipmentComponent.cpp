#include "Equipment/FretteEquipmentComponent.h"

#include "Frette/Frette.h"
#include "Inventory/Items/FretteSlottedItem.h"
#include "Player/FrettePlayerState.h"

void UFretteEquipmentComponent::Initialize(const AFrettePlayerState* PlayerState)
{
	PlayerState->GetPlayerInventory()->OnItemSelected.AddUObject(this, &UFretteEquipmentComponent::OnInventoryItemSelected);
}

void UFretteEquipmentComponent::OnInventoryItemSelected(const UFretteInventoryItem* Item)
{
	if (Item->IsA<UFretteSlottedItem>())
	{
		UE_LOG(LogFrette, Log, TEXT("Equipment: Received new item '%s'."), *Item->Data->DisplayName.ToString());
	}
}