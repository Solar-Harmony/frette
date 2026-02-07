#include "Inventory/SlotsInventoryComponent.h"

int32 USlotsInventoryComponent::GetItemCount(UInventoryItemDataAsset* ItemType)
{
	if (!ItemType)
	{
		return Items.Num();
	}

	int32 Count = 0;
	for (const UInventoryItem* Item : Items)
	{
		if (Item && Item->Data == ItemType)
		{
			Count++;
		}
	}
	return Count;
}

UInventoryItem* USlotsInventoryComponent::GetItem(int32 Index)
{
	return Items.IsValidIndex(Index) ? Items[Index] : nullptr;
}

int32 USlotsInventoryComponent::AddItem(UInventoryItemDataAsset* ItemData)
{
	check(ItemData);

	UInventoryItem* NewItem = ItemData->CreateRuntimeItem(this);
	check(NewItem);

	const int32 Index = Items.Add(NewItem);
	OnItemAdded.Broadcast(NewItem);
	return Index;
}

void USlotsInventoryComponent::RemoveItem(int32 Index)
{
	if (!Items.IsValidIndex(Index))
		return;

	UInventoryItem* Item = Items[Index];
	check(Item);

	Items.RemoveAt(Index);
	// TODO: this must be in OnRep to work with replication
	OnItemRemoved.Broadcast(Item);
}

void USlotsInventoryComponent::SwapItems(int32 FromIndex, int32 ToIndex)
{
	if (Items.IsValidIndex(FromIndex) && Items.IsValidIndex(ToIndex))
	{
		Items.Swap(FromIndex, ToIndex);
	}
}