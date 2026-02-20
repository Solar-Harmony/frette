#include "Inventory/FretteInventoryComponent.h"

#include "Engine/World.h"
#include "Frette/Frette.h"
#include "GameFramework/PlayerController.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UFretteInventoryComponent::SelectItem(int32 ItemId) const
{
	const UFretteInventoryItem* ItemToSelect = GetItem(ItemId);
	require(ItemToSelect, "Inventory: Cannot select item #%d because it was not found in this inventory.", ItemId);

	OnItemSelected.Broadcast(ItemToSelect);
	K2_OnItemSelected.Broadcast(ItemToSelect);
}

void UFretteInventoryComponent::AddItem_Implementation(UFretteInventoryItemDataAsset* ItemData)
{
	require(IsReadyForReplication() && GetOwner()->HasAuthority());
	require(IsValid(ItemData), "Inventory: Cannot add item: item data asset is invalid.");

	UFretteInventoryItem* Item = ItemData->CreateRuntimeItem(this);
	AddReplicatedSubObject(Item);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::ChangeItem_Implementation(UFretteInventoryItem* ItemToChange)
{
	require(GetOwner()->HasAuthority());
	require(Inventory.IsValidItem(ItemToChange), "Inventory: Cannot change item because it is invalid.");
	Inventory.ChangeEntry(ItemToChange);
}

void UFretteInventoryComponent::RemoveItem_Implementation(int32 ItemId)
{
	require(GetOwner()->HasAuthority());
	require(Inventory.HasEntry(ItemId), "Inventory: Cannot remove item #%d because this inventory has no item with that ID.", ItemId);

	Inventory.RemoveEntry(ItemId);
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	for (TObjectPtr StartingItemData : StartingItems)
	{
		AddItem(StartingItemData);
	}
}