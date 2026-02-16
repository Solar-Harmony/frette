#include "Inventory/FretteInventoryList.h"

#include "Inventory/FretteInventoryComponent.h"

UFretteInventoryItem* FFretteInventoryList::GetEntry(int32 Index) const
{
	check(Entries.IsValidIndex(Index));
	return Entries[Index].Item;
}

void FFretteInventoryList::AddEntry(UFretteInventoryItem* ItemToAdd)
{
	check(Owner->GetOwner()->HasAuthority());
	check(IsValidItem(ItemToAdd));

	FFretteInventoryListEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = ItemToAdd;
	Entry.Item->Id = NextId++;
	IdToIndexMap.Add(Entry.Item->Id, Entries.Num() - 1);
	MarkItemDirty(Entry);
}

void FFretteInventoryList::ChangeEntry(UFretteInventoryItem* ItemToChange)
{
	check(Owner->GetOwner()->HasAuthority());

	const int32 Idx = GetItemIndexChecked(ItemToChange);
	FFretteInventoryListEntry& Entry = Entries[Idx];
	Entry.Item = ItemToChange;
	MarkItemDirty(Entry);
}

void FFretteInventoryList::RemoveEntry(const UFretteInventoryItem* ItemToRemove)
{
	check(Owner->GetOwner()->HasAuthority());

	const int32 Idx = GetItemIndexChecked(ItemToRemove);
	const int32 LastIdx = Entries.Num() - 1;
	const FFretteInventoryListEntry& EntryToRemove = Entries[Idx];

	if (Idx != LastIdx)
	{
		const FFretteInventoryListEntry& EntryToMove = Entries[LastIdx];
		IdToIndexMap[EntryToMove.Item->Id] = Idx;
		Entries.Swap(Idx, LastIdx);
	}

	IdToIndexMap.Remove(EntryToRemove.Item->Id);
	Entries.RemoveAt(LastIdx);
	MarkArrayDirty();
}

bool FFretteInventoryList::IsValidItem(const UFretteInventoryItem* Item) const
{
	if (!IsValid(Item))
		return false;

	if (Item->GetOuter() != Owner)
		return false; // not from the same inventory

	if (Item->Id == InvalidID)
		return false; // was never added to inventory

	if (!IsValid(Item->Data))
		return false;

	return true;
}

int32 FFretteInventoryList::GetItemIndexChecked(const UFretteInventoryItem* Item) const
{
	check(IsValidItem(Item));

	const int32& Idx = IdToIndexMap.FindChecked(Item->Id);
	check(Entries.IsValidIndex(Idx));
	check(Entries[Idx].Item == Item);
	return Idx;
}

void FFretteInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FFretteInventoryListEntry& Stack = Entries[Index];
		IdToIndexMap.Add(Stack.Item->Id, Index);
		Owner->OnItemAdded.Broadcast(Stack.Item);
	}
}

void FFretteInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// TODO: nothing for now
}

void FFretteInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		const FFretteInventoryListEntry& Stack = Entries[Index];
		IdToIndexMap.Remove(Stack.Item->Id);
		Owner->OnItemRemoved.Broadcast(Stack.Item);
	}
}