#include "Inventory/FretteInventoryComponent.h"

#include "Frette/Frette.h"
#include "Net/UnrealNetwork.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

bool UFretteInventoryComponent::HasItem(int32 Id) const
{
	return Inventory.HasEntry(Id);
}

UFretteInventoryItem* UFretteInventoryComponent::GetItem(int32 Id) const
{
	return Inventory.GetItemById(Id);
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
	require(Inventory.IsValidItem(ItemToChange), "#%d", ItemToChange->Id);
	Inventory.ChangeEntry(ItemToChange);
}

void UFretteInventoryComponent::RemoveItem_Implementation(UFretteInventoryItem* ItemToRemove)
{
	require(GetOwner()->HasAuthority());
	require(Inventory.IsValidItem(ItemToRemove), "#%d", ItemToRemove->Id);

	RemoveReplicatedSubObject(ItemToRemove);
	Inventory.RemoveEntry(ItemToRemove);
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	for (TObjectPtr StartingItemData : StartingItems)
	{
		AddItem(StartingItemData);
	}
}

void UFretteInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFretteInventoryComponent, Inventory, COND_OwnerOnly);
}