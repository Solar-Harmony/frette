#include "Inventory/InventoryList.h"

TArray<UInventoryItem*> FInventoryList::GetAllItems() const
{
	TArray<UInventoryItem*> Results;
	Results.Reserve(Entries.Num());

	for (const FInventoryListEntry& Entry : Entries)
	{
		if (Entry.Item != nullptr)
		{
			Results.Add(Entry.Item);
		}
	}
	return Results;
}

void FInventoryList::AddEntry(UInventoryItem* Instance)
{
	check(Instance);
	check(Instance->Data);
	check(Owner);

	FInventoryListEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = Instance;
	MarkItemDirty(Entry);

	Cast<IInventoryComponent>(Owner)->OnItemAdded.Broadcast(Entry.Item);
}

UInventoryItem* FInventoryList::AddEntry(UInventoryItemDataAsset* ItemClass)
{
	check(ItemClass);
	check(Owner);

	AActor* OwningActor = Owner->GetOwner();
	check(OwningActor->HasAuthority());

	FInventoryListEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = ItemClass->CreateRuntimeItem(Owner);
	MarkItemDirty(Entry);

	if (Owner)
	{
		Cast<IInventoryComponent>(Owner)->OnItemAdded.Broadcast(Entry.Item);
	}

	return Entry.Item;
}

void FInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// TODO: nothing for now
}

void FInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FInventoryListEntry& Stack = Entries[Index];
		Cast<IInventoryComponent>(Owner)->OnItemAdded.Broadcast(Stack.Item);
	}
}

void FInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// TODO: nothing for now
}