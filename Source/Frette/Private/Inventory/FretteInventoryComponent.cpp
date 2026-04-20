#include "Inventory/FretteInventoryComponent.h"

#include "Engine/World.h"
#include "Frette/Frette.h"
#include "GameFramework/PlayerController.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	
	if (GetOwner()->HasAuthority())
	{
		for (TObjectPtr StartingItemData : StartingItems)
		{
			AddItem(StartingItemData);
		}
	}
}

UFretteInventoryItem* UFretteInventoryComponent::GetItemByIndexEditor(int32 Idx) const
{
	return Inventory.GetItemByIndex(Idx);
}

void UFretteInventoryComponent::UseItem_Implementation(int32 ItemId)
{
	UFretteInventoryItem* ItemToUse = GetItem(ItemId);
	precondition(ItemToUse, "Inventory: Cannot select item #%d because it was not found in this inventory.", ItemId);

	ItemToUse->Use();
	
	if (ItemToUse->Data->bConsumeOnUse)
	{
		RemoveItem_Implementation(ItemId);
	}
	
	OnItemSelected.Broadcast(ItemToUse);
	K2_OnItemSelected.Broadcast(ItemToUse);
}

void UFretteInventoryComponent::AddItem_Implementation(UFretteInventoryItemDataAsset* ItemData)
{
	precondition(IsReadyForReplication() && GetOwner()->HasAuthority());
	precondition(IsValid(ItemData), "Inventory: Cannot add item: item data asset is invalid.");

	UFretteInventoryItem* Item = ItemData->CreateRuntimeItem(this);
	AddReplicatedSubObject(Item);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::ChangeItem_Implementation(UFretteInventoryItem* ItemToChange)
{
	precondition(GetOwner()->HasAuthority());
	precondition(Inventory.IsValidItem(ItemToChange), "Inventory: Cannot change item because it is invalid.");
	Inventory.ChangeEntry(ItemToChange);
}

void UFretteInventoryComponent::RemoveItem_Implementation(int32 ItemId)
{
	precondition(GetOwner()->HasAuthority());
	precondition(Inventory.HasEntry(ItemId), "Inventory: Cannot remove item #%d because this inventory has no item with that ID.", ItemId);

	Inventory.RemoveEntry(ItemId);
}