// 

#include "InventoryActorComponent.h"

#include "IDetailTreeNode.h"
#include "Algo/Count.h"
#include "Engine/AssetManager.h"

UInventoryActorComponent::UInventoryActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	if (!bUseArea)
		Items.Reserve(MaxNumItems);
	
	BadGuid.Invalidate();
}

void UInventoryActorComponent::BeginPlay()
{
	Super::BeginPlay();

	UAssetManager& Manager = UAssetManager::Get();
    
	Manager.LoadPrimaryAssetsWithType(
		Item_Type, 
		TArray<FName>(),
		FStreamableDelegate::CreateUObject(this, &UInventoryActorComponent::OnItemDataAssetsLoaded)
	);
}

void UInventoryActorComponent::OnItemDataAssetsLoaded()
{
	const UAssetManager& Manager = UAssetManager::Get();
    
	TArray<FPrimaryAssetId> ItemIds;
	Manager.GetPrimaryAssetIdList(Item_Type, ItemIds);

	for (const FPrimaryAssetId& ItemId : ItemIds)
	{
		UItemDataAsset* Data = Cast<UItemDataAsset>(Manager.GetPrimaryAssetObject(ItemId));
		if (Data)
			ItemIdLookup.FindOrAdd(ItemId.PrimaryAssetName, Data);
	}
	
	OnInventoryReady.Broadcast();
}

void UInventoryActorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FGuid UInventoryActorComponent::AddItem(FName ItemId)
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData || IsFull())
		return BadGuid;
	
	FInventoryItem NewItem(*MaybeData);
	Items.Add(NewItem);
	return NewItem.InstanceId;
}
	
TArray<FGuid> UInventoryActorComponent::AddItems(FName ItemId, int32 Quantity)
{
	TArray<FGuid> NewItemsGuids;
	for (int32 i = 0; i < Quantity && !IsFull(); ++i)
		NewItemsGuids.Add(AddItem(ItemId));
	return NewItemsGuids;
}

int32 UInventoryActorComponent::GetTotalNumItems() const
{
	return Items.Num();
}

int32 UInventoryActorComponent::GetNumItems(FName ItemId) const
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return 0;
	
	return Algo::CountIf(Items, [MaybeData](const FInventoryItem& Item)
	{
		return Item.Data == *MaybeData;
	});
}

bool UInventoryActorComponent::IsFull() const
{
	if (bUseArea)
	{
		return Algo::Accumulate(Items, 0, [](int32 sum, const FInventoryItem& Item)
		{
			return sum + Item.Data->Area;
		}) >= MaxArea;
	}
	
	return Items.Num() >= MaxNumItems;
}

bool UInventoryActorComponent::HasItem(FName ItemId) const
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return false;
	
	return Items.ContainsByPredicate([MaybeData](const FInventoryItem& Item)
	{
		return Item.Data == *MaybeData;
	});
}

FGuid UInventoryActorComponent::GetFirstItem(FName ItemId) const
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return BadGuid;
	
	const FInventoryItem* MaybeItem = Items.FindByPredicate([MaybeData](const FInventoryItem& Item)
	{
		return Item.Data == *MaybeData;
	});
	
	if (!MaybeItem) 
		return BadGuid;
	
	return MaybeItem->InstanceId;
}
	
TArray<FGuid> UInventoryActorComponent::GetItems(FName ItemId) const
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
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

bool UInventoryActorComponent::RemoveItem(FGuid ItemGuid)
{
	return Items.RemoveAll([ItemGuid](const FInventoryItem& Item)
	{
		return Item.InstanceId == ItemGuid;
	}) > 0;
}
	
int32 UInventoryActorComponent::RemoveItems(const TArray<FGuid> &ItemGuids)
{
	return Items.RemoveAll([&ItemGuids](const FInventoryItem& Item)
	{
		return ItemGuids.Contains(Item.InstanceId);
	});
}
	
int32 UInventoryActorComponent::RemoveItemsOfId(FName ItemId)
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return 0;
	
	return Items.RemoveAll([MaybeData](const FInventoryItem& Item)
	{
		return Item.Data == *MaybeData;
	});
}

bool UInventoryActorComponent::WithItem(FGuid ItemGuid, TFunctionRef<void(FInventoryItem&)> Func)
{
	FInventoryItem* MaybeItem = Items.FindByPredicate([ItemGuid, Func](const FInventoryItem& Item)
	{
		return Item.InstanceId == ItemGuid;
	});
	
	if (MaybeItem)
	{
		Func(*MaybeItem);
		return true;
	}
	return false;
}
	
int32 UInventoryActorComponent::WithItems(const TArray<FGuid>& ItemGuids, const TFunctionRef<void(FInventoryItem&)>& Func)
{
	int32 ModifiedCount = 0;
	for (const FGuid ItemGuid : ItemGuids)
		 ModifiedCount += WithItem(ItemGuid, Func) ? 1 : 0;
	return ModifiedCount;
}

FInventoryItem UInventoryActorComponent::GetItemCopy(FGuid ItemGuid) const
{
	const FInventoryItem* MaybeItem = Items.FindByPredicate([ItemGuid](const FInventoryItem& Item)
	{
		return Item.InstanceId == ItemGuid;
	});
	return MaybeItem ? *MaybeItem : FInventoryItem();
}
	
TArray<FInventoryItem> UInventoryActorComponent::GetItemCopies(const TArray<FGuid>& ItemGuids) const 
{
	TArray<FInventoryItem> ItemCopies = Items.FilterByPredicate([ItemGuids](const FInventoryItem& Item)
	{
		return ItemGuids.Contains(Item.InstanceId);
	});
	return ItemCopies;
}

bool UInventoryActorComponent::SetItem(const FInventoryItem& NewItemData)
{
	FGuid ItemGuid = NewItemData.InstanceId;
	int32 Index = Items.IndexOfByPredicate([ItemGuid](const FInventoryItem& Item)
	{
		return Item.InstanceId == ItemGuid;
	});
	
	if (Index == INDEX_NONE && IsFull())
		return false;
	if (Index != INDEX_NONE)
		Items.RemoveAt(Index);
	
	Items.Add(NewItemData);
	return true;
}
	
int UInventoryActorComponent::SetItems(const TArray<FInventoryItem>& NewItemsData)
{
	int32 ModifiedCount = 0;
	for (const FInventoryItem& NewItem : NewItemsData)
		ModifiedCount += SetItem(NewItem) ? 1 : 0;
	return ModifiedCount; 
}

FInventoryItem UInventoryActorComponent::CreateItemCopy(FName ItemId) const
{
	UItemDataAsset* const* MaybeData = ItemIdLookup.Find(ItemId);
	if (!MaybeData)
		return FInventoryItem();
	
	return FInventoryItem(*MaybeData);
}