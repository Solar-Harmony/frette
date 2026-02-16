#include "Inventory/FretteInventoryList.h"

#include "Inventory/FretteInventoryComponent.h"

UFretteInventoryItem* FFretteInventoryList::GetItemById(int32 ItemId) const
{
	return Entries[GetIndexByIdChecked(ItemId)].Item;
}

void FFretteInventoryList::AddEntry(UFretteInventoryItem* ItemToAdd)
{
	check(Owner->GetOwner()->HasAuthority());
	check(HasValidItemData(ItemToAdd));

	FFretteInventoryListEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = ItemToAdd;
	Entry.Item->Id = NextId++;
	IdToIndexMap.Add(Entry.Item->Id, Entries.Num() - 1);
	MarkItemDirty(Entry);
}

void FFretteInventoryList::ChangeEntry(UFretteInventoryItem* ItemToChange)
{
	check(Owner->GetOwner()->HasAuthority());
	check(HasValidItemData(ItemToChange));

	const int32 Idx = GetIndexByIdChecked(ItemToChange->Id);
	FFretteInventoryListEntry& Entry = Entries[Idx];
	Entry.Item = ItemToChange;
	MarkItemDirty(Entry);
}

void FFretteInventoryList::RemoveEntry(const UFretteInventoryItem* ItemToRemove)
{
	check(Owner->GetOwner()->HasAuthority());
	check(HasValidItemData(ItemToRemove));

	const int32 Idx = GetIndexByIdChecked(ItemToRemove->Id);
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

bool FFretteInventoryList::HasValidItemData(const UFretteInventoryItem* Item) const
{
	if (!IsValid(Item))
		return false;

	if (Item->GetOuter() != Owner)
		return false; // not from the same inventory

	if (!IsValid(Item->Data))
		return false;

	return true;
}

int32 FFretteInventoryList::GetIndexByIdChecked(int32 ItemId) const
{
	const int32& Idx = IdToIndexMap.FindChecked(ItemId);
	check(Entries.IsValidIndex(Idx));
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
	for (const int32 Index : ChangedIndices)
	{
		Owner->OnItemChanged.Broadcast(Entries[Index].Item);
	}
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