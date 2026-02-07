#include "Inventory/StacksInventoryComponent.h"

// we could pre-fill the map with all possible items using asset manager
// but we would need to do it before the inventory is used, e.g. in gameinstance
// this is simpler and doesn't really matter, although we lose const correctness
UInventoryItem* UStacksInventoryComponent::GetItem(UInventoryItemDataAsset* ItemType)
{
	ensureAlwaysMsgf(ItemType, TEXT("%s: Missing ItemType."), *FString(__FUNCTION__));

	const FPrimaryAssetId Id = ItemType->GetPrimaryAssetId();
	if (TObjectPtr<UInventoryItem>* FoundItem = Items.Find(Id))
	{
		return *FoundItem;
	}

	UInventoryItem* NewItem = ItemType->CreateRuntimeItem(this);
	Items.Add(Id, NewItem);
	return NewItem;
}

int32 UStacksInventoryComponent::GetItemCount(UInventoryItemDataAsset* ItemType)
{
	ensureAlwaysMsgf(ItemType, TEXT("%s: Missing ItemType."), *FString(__FUNCTION__));

	const FPrimaryAssetId Id = ItemType->GetPrimaryAssetId();
	if (const TObjectPtr<UInventoryItem>* FoundItem = Items.Find(Id))
	{
		return (*FoundItem)->Quantity;
	}

	return 0;
}

UInventoryItem* UStacksInventoryComponent::AddItem(UInventoryItemDataAsset* ItemType, int32 Quantity)
{
	UInventoryItem* Item = GetItem(ItemType);
	check(Item);

	Item->Quantity += Quantity;
	OnItemAdded.Broadcast(Item);

	return Item;
}

bool UStacksInventoryComponent::RemoveItem(UInventoryItemDataAsset* ItemType, int32 Quantity)
{
	UInventoryItem* Item = GetItem(ItemType);
	check(Item);

	if (Item->Quantity == 0)
		return false;

	Item->Quantity = FMath::Max(0, Item->Quantity - Quantity);
	OnItemRemoved.Broadcast(Item);
	return true;
}