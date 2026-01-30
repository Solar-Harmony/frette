// 


#include "InventoryActorComponent.h"

#include "IDetailTreeNode.h"
#include "Algo/Count.h"
#include "Engine/AssetManager.h"


// Sets default values for this component's properties
UInventoryActorComponent::UInventoryActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
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
		ITEM_TYPE, 
		TArray<FName>(),
		FStreamableDelegate::CreateUObject(this, &UInventoryActorComponent::OnItemDataAssetsLoaded)
	);
}

void UInventoryActorComponent::OnItemDataAssetsLoaded()
{
	UAssetManager& Manager = UAssetManager::Get();
    
	TArray<FPrimaryAssetId> ItemIds;
	Manager.GetPrimaryAssetIdList(ITEM_TYPE, ItemIds);

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
