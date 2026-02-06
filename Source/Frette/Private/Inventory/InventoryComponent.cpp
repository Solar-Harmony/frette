#include "Inventory/InventoryComponent.h"

FInventoryItem UInventoryComponent::AddItem(UInventoryItemDataAsset* ItemType)
{
	ensureAlways(ItemType);

	const FInventoryItem ItemToAdd{ ItemType };
	Items.AddUnique(ItemType->GetPrimaryAssetId(), ItemToAdd);

	return ItemToAdd;
}

TOptional<FInventoryItem> UInventoryComponent::FindItemSafe(const UInventoryItemDataAsset* ItemType, const TFunctionRef<FInventoryItem*()>& Predicate)
{
	FInventoryItem* FoundItem = FindItem(ItemType, Predicate);
	return FoundItem ? TOptional(*FoundItem) : TOptional<FInventoryItem>();
}

FInventoryQueryResult UInventoryComponent::K2_FindItem(UInventoryItemDataAsset* ItemType, FInventoryQuery Query) const
{
	FoundItems.Reset(1);
	Items.MultiFind(ItemType->GetPrimaryAssetId(), FoundItems);

	if (FoundItems.IsEmpty())
	{
		return { false, FInventoryItem() };
	}

	if (Query.Attribute == EInventoryQueryAttribute::FirstFound)
		return { true, FoundItems[0] };

	auto MatchesPredicate = [Query](const FInventoryItem& Item) -> bool {
		const float AttributeValue = Item.GetAttributeValue(Query.Attribute);
		return CompareFloat(AttributeValue, Query.Value, Query.Predicate);
	};

	const FInventoryItem* Item = FoundItems.FindByPredicate(MatchesPredicate);
	return { Item != nullptr, Item ? *Item : FInventoryItem() };
}

FInventoryItem* UInventoryComponent::FindItem(const UInventoryItemDataAsset* ItemType, const TFunctionRef<bool(FInventoryItem*)>& Predicate) const
{
	FoundItems.Reset(1);
	Items.MultiFind(ItemType->GetPrimaryAssetId(), FoundItems);

	if (FoundItems.IsEmpty())
		return nullptr;

	FInventoryItem* FoundItem = FoundItems.FindByPredicate(Predicate);
	if (FoundItem == nullptr)
		return nullptr;

	return FoundItem;
}