#include "Inventory/InventoryComponent.h"

#include "Algo/Count.h"
#include "Engine/AssetManager.h"

UInventoryComponent::UInventoryComponent()
{
	if (!bUseArea)
		Items.Reserve(MaxNumItems);

	BadGuid.Invalidate();
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	UAssetManager& Manager = UAssetManager::Get();

	// FIXME: This will run for each inventory component, but it should only run once. 
	// An easy fix: make the ItemIdLookup static and only load assets if it's empty.
	// Will matter if NPCs etc. start having inventories, but for now it's fine since only the player has one.
	Manager.LoadPrimaryAssetsWithType(
		GInventoryItemPrimaryAssetType,
		TArray<FName>(),
		FStreamableDelegate::CreateUObject(this, &UInventoryComponent::OnItemDataAssetsLoaded)
		);
}

void UInventoryComponent::OnItemDataAssetsLoaded()
{
	const UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> ItemIds;
	Manager.GetPrimaryAssetIdList(GInventoryItemPrimaryAssetType, ItemIds);

	for (const FPrimaryAssetId& ItemId : ItemIds)
	{
		if (UInventoryItemDataAsset* Data = Cast<UInventoryItemDataAsset>(Manager.GetPrimaryAssetObject(ItemId)))
			ItemIdLookup.FindOrAdd(ItemId.PrimaryAssetName, Data);
	}

	OnInventoryReady.Broadcast();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FGuid UInventoryComponent::AddItem(FName ItemId)
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData || IsFull())
		return BadGuid;

	FInventoryItem NewItem(*MaybeData);
	Items.Add(NewItem);
	return NewItem.InstanceId;
}

TArray<FGuid> UInventoryComponent::AddItems(FName ItemId, int32 Quantity)
{
	TArray<FGuid> NewItemsGuids;
	for (int32 i = 0; i < Quantity && !IsFull(); ++i)
		NewItemsGuids.Add(AddItem(ItemId));
	return NewItemsGuids;
}

int32 UInventoryComponent::GetTotalNumItems() const
{
	return Items.Num();
}

int32 UInventoryComponent::GetNumItems(FName ItemId) const
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return 0;

	return Algo::CountIf(Items, [MaybeData](const FInventoryItem& Item) {
		return Item.Data == *MaybeData;
	});
}

bool UInventoryComponent::IsFull() const
{
	if (bUseArea)
	{
		return Algo::Accumulate(Items, 0, [](int32 sum, const FInventoryItem& Item) {
			return sum + Item.Data->Area;
		}) >= MaxArea;
	}

	return Items.Num() >= MaxNumItems;
}

bool UInventoryComponent::HasItem(FName ItemId) const
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return false;

	return Items.ContainsByPredicate([MaybeData](const FInventoryItem& Item) {
		return Item.Data == *MaybeData;
	});
}

FGuid UInventoryComponent::GetFirstItem(FName ItemId) const
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return BadGuid;

	const FInventoryItem* MaybeItem = Items.FindByPredicate([MaybeData](const FInventoryItem& Item) {
		return Item.Data == *MaybeData;
	});

	if (!MaybeItem)
		return BadGuid;

	return MaybeItem->InstanceId;
}

TArray<FGuid> UInventoryComponent::GetItems(FName ItemId) const
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return TArray<FGuid>();

	TArray<FGuid> Guids;
	for (const FInventoryItem& Item : Items)
	{
		if (Item.Data == *MaybeData)
			Guids.Add(Item.InstanceId);
	}
	return Guids;
}

bool UInventoryComponent::RemoveItem(FGuid ItemGuid)
{
	return Items.RemoveAll([ItemGuid](const FInventoryItem& Item) {
		return Item.InstanceId == ItemGuid;
	}) > 0;
}

int32 UInventoryComponent::RemoveItems(const TArray<FGuid>& ItemGuids)
{
	return Items.RemoveAll([&ItemGuids](const FInventoryItem& Item) {
		return ItemGuids.Contains(Item.InstanceId);
	});
}

int32 UInventoryComponent::RemoveItemsOfId(FName ItemId)
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return 0;

	return Items.RemoveAll([MaybeData](const FInventoryItem& Item) {
		return Item.Data == *MaybeData;
	});
}

bool UInventoryComponent::WithItem(FGuid ItemGuid, TFunctionRef<void(FInventoryItem&)> Func)
{
	FInventoryItem* MaybeItem = Items.FindByPredicate([ItemGuid, Func](const FInventoryItem& Item) {
		return Item.InstanceId == ItemGuid;
	});

	if (MaybeItem)
	{
		Func(*MaybeItem);
		return true;
	}
	return false;
}

int32 UInventoryComponent::WithItems(const TArray<FGuid>& ItemGuids, const TFunctionRef<void(FInventoryItem&)>& Func)
{
	int32 ModifiedCount = 0;
	for (const FGuid ItemGuid : ItemGuids)
		ModifiedCount += WithItem(ItemGuid, Func) ? 1 : 0;
	return ModifiedCount;
}

FInventoryItem UInventoryComponent::GetItemCopy(FGuid ItemGuid) const
{
	const FInventoryItem* MaybeItem = Items.FindByPredicate([ItemGuid](const FInventoryItem& Item) {
		return Item.InstanceId == ItemGuid;
	});
	return MaybeItem ? *MaybeItem : FInventoryItem();
}

TArray<FInventoryItem> UInventoryComponent::GetItemCopies(const TArray<FGuid>& ItemGuids) const
{
	TArray<FInventoryItem> ItemCopies = Items.FilterByPredicate([ItemGuids](const FInventoryItem& Item) {
		return ItemGuids.Contains(Item.InstanceId);
	});
	return ItemCopies;
}

bool UInventoryComponent::SetItem(const FInventoryItem& NewItemData)
{
	FGuid ItemGuid = NewItemData.InstanceId;
	int32 Index = Items.IndexOfByPredicate([ItemGuid](const FInventoryItem& Item) {
		return Item.InstanceId == ItemGuid;
	});

	if (Index == INDEX_NONE && IsFull())
		return false;
	if (Index != INDEX_NONE)
		Items.RemoveAt(Index);

	Items.Add(NewItemData);
	return true;
}

int UInventoryComponent::SetItems(const TArray<FInventoryItem>& NewItemsData)
{
	int32 ModifiedCount = 0;
	for (const FInventoryItem& NewItem : NewItemsData)
		ModifiedCount += SetItem(NewItem) ? 1 : 0;
	return ModifiedCount;
}

FInventoryItem UInventoryComponent::CreateItemCopy(FName ItemId) const
{
	UInventoryItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return FInventoryItem();

	return FInventoryItem(*MaybeData);
}