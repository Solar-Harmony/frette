#include "Inventory/FretteInventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Frette/Frette.h"
#include "Net/UnrealNetwork.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

UFretteInventoryItem* UFretteInventoryComponent::GetItem(int32 Id) const
{
	return Inventory.GetItemById(Id);
}

void UFretteInventoryComponent::SelectItem(const UFretteInventoryItem* Item) const
{
	if (!Inventory.HasValidItemData(Item))
	{
		LOG_FRETTE(Warning, "Inventory: Selected item is invalid");
	}

	OnItemSelected.Broadcast(Item);
}

void UFretteInventoryComponent::AddItem_Implementation(UFretteInventoryItemDataAsset* ItemData)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (!IsValid(ItemData))
	{
		LOG_FRETTE(Warning, "Inventory: Invalid data asset passed to AddItem.");
		return;
	}

	UFretteInventoryItem* Item = ItemData->CreateRuntimeItem(this);
	AddReplicatedSubObject(Item);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::ChangeItem_Implementation(UFretteInventoryItem* ItemToChange)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (!Inventory.HasValidItemData(ItemToChange))
	{
		LOG_FRETTE(Warning, "Inventory: Attempted to change an invalid item.");
		return;
	}

	Inventory.ChangeEntry(ItemToChange);
}

void UFretteInventoryComponent::RemoveItem_Implementation(UFretteInventoryItem* ItemToRemove)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (!Inventory.HasValidItemData(ItemToRemove))
	{
		LOG_FRETTE(Warning, "Inventory: Attempted to remove an invalid item.");
		return;
	}

	RemoveReplicatedSubObject(ItemToRemove);
	Inventory.RemoveEntry(ItemToRemove);
}

void UFretteInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFretteInventoryComponent, Inventory, COND_OwnerOnly);
}