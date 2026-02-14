#include "Inventory/FretteInventoryList.h"

#include "Frette/Frette.h"
#include "Inventory/FretteInventoryComponent.h"

UFretteInventoryItem* FFretteInventoryList::GetEntry(int32 Index)
{
	if (!Entries.IsValidIndex(Index))
	{
		UE_LOG(LogFrette, Warning, TEXT("Inventory: Out of bounds GetEntry at index %d"), Index);
		return nullptr;
	}

	return Entries[Index].Item;
}

void FFretteInventoryList::AddEntry(UFretteInventoryItem* Instance)
{
	check(Instance);
	check(Instance->Data);
	check(Owner);
	check(Owner->GetOwner()->HasAuthority());

	FFretteInventoryListEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = Instance;
	MarkItemDirty(Entry);
}

void FFretteInventoryList::ChangeEntry(int32 Index, UFretteInventoryItem* NewInstance)
{
	check(Entries.IsValidIndex(Index));	
	check(NewInstance);
	check(NewInstance->Data);
	check(Owner);
	check(Owner->GetOwner()->HasAuthority());

	FFretteInventoryListEntry& Entry = Entries[Index];
	Entry.Item = NewInstance;
	MarkItemDirty(Entry);
}

void FFretteInventoryList::RemoveEntry(int32 Index)
{
	check(Entries.IsValidIndex(Index));
	check(Owner);
	check(Owner->GetOwner()->HasAuthority());

	UFretteInventoryItem* Item = Entries[Index].Item;
	Entries.RemoveAt(Index);
	MarkArrayDirty();
}

void FFretteInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		const FFretteInventoryListEntry& Stack = Entries[Index];
		Owner->OnItemRemoved.Broadcast(Stack.Item);
	}
}

void FFretteInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FFretteInventoryListEntry& Stack = Entries[Index];
		Owner->OnItemAdded.Broadcast(Stack.Item);
	}
}

void FFretteInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// TODO: nothing for now
}