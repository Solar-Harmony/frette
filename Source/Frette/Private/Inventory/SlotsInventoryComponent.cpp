#include "Inventory/SlotsInventoryComponent.h"

#include "Net/UnrealNetwork.h"

USlotsInventoryComponent::USlotsInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	Inventory.Owner = this;
}

int32 USlotsInventoryComponent::GetItemCount(UInventoryItemDataAsset* ItemType)
{
	if (!ItemType)
	{
		return Inventory.Items.Num();
	}

	return 67;
}

UInventoryItem* USlotsInventoryComponent::GetItem(int32 Index)
{
	return Inventory.Items.IsValidIndex(Index) ? Inventory.Items[Index].Item : nullptr;
}

int32 USlotsInventoryComponent::AddItem(UInventoryItemDataAsset* ItemData)
{
	check(ItemData);

	UInventoryItem* NewItem = ItemData->CreateRuntimeItem(this);
	check(NewItem);
	
	FInventoryListEntry Entry;
	Entry.Item = NewItem;
	const int32 Index = Inventory.Items.Add(MoveTemp(Entry));
	Inventory.MarkItemDirty(Entry);
	return Index;
}

void USlotsInventoryComponent::RemoveItem(int32 Index)
{
	if (!Inventory.Items.IsValidIndex(Index))
		return;
	
	Inventory.Items.RemoveAt(Index);
	Inventory.MarkArrayDirty();
}

void USlotsInventoryComponent::SwapItems(int32 FromIndex, int32 ToIndex)
{
	if (Inventory.Items.IsValidIndex(FromIndex) && Inventory.Items.IsValidIndex(ToIndex))
	{
		Inventory.Items.Swap(FromIndex, ToIndex);
	}
}

void USlotsInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USlotsInventoryComponent, Inventory);
}