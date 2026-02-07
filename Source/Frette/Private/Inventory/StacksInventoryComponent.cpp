#include "Inventory/StacksInventoryComponent.h"
#include "InstancedStruct.h"

// we could pre-fill the map with all possible items using asset manager
// but we would need to do it before the inventory is used, e.g. in gameinstance
// this is simpler and doesn't really matter, although we lose const correctness
FStackedInventoryItem& UStacksInventoryComponent::GetOrAddItem(UInventoryItemDataAsset* ItemType)
{
	return Items.FindOrAdd(ItemType->GetPrimaryAssetId(), FStackedInventoryItem{ ItemType, 0 });
}

int32 UStacksInventoryComponent::GetItemCount(UInventoryItemDataAsset* ItemType)
{
	const FStackedInventoryItem& Stack = GetOrAddItem(ItemType);
	return Stack.Quantity;
}

FStackedInventoryItem& UStacksInventoryComponent::AddItem(UInventoryItemDataAsset* ItemType, int32 Quantity)
{
	FStackedInventoryItem& Stack = GetOrAddItem(ItemType);
	Stack.Quantity += Quantity;
	OnItemAdded.Broadcast(FInstancedStruct::Make(Stack));
	return Stack;
}

bool UStacksInventoryComponent::RemoveItem(UInventoryItemDataAsset* ItemType, int32 Quantity)
{
	FStackedInventoryItem& Stack = GetOrAddItem(ItemType);
	if (Stack.Quantity == 0)
		return false;

	Stack.Quantity = FMath::Max(0, Stack.Quantity - Quantity);
	OnItemRemoved.Broadcast(FInstancedStruct::Make(Stack));
	return true;
}