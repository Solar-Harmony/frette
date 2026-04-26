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
	unless(ItemToUse, "Inventory: Cannot select item #%d because it was not found in this inventory.", ItemId)
		return;

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
	unless(IsReadyForReplication() && GetOwner()->HasAuthority()) 
		return;
	
	unless(IsValid(ItemData), "Inventory: Cannot add item: item data asset is invalid.") 
		return;

	UFretteInventoryItem* Item = ItemData->CreateRuntimeItem(this);
	AddReplicatedSubObject(Item);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::ChangeItem_Implementation(UFretteInventoryItem* ItemToChange)
{
	unless(GetOwner()->HasAuthority()) 
		return;
	
	unless(Inventory.IsValidItem(ItemToChange), "Inventory: Cannot change item because it is invalid.")
		return;
	
	Inventory.ChangeEntry(ItemToChange);
}

void UFretteInventoryComponent::RemoveItem_Implementation(int32 ItemId)
{
	unless(GetOwner()->HasAuthority()) 
		return;
	
	unless(Inventory.HasEntry(ItemId), "Inventory: Cannot remove item #%d because this inventory has no item with that ID.", ItemId)
		return;

	Inventory.RemoveEntry(ItemId);
}