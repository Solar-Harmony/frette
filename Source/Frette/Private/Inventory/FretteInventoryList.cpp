#include "Inventory/FretteInventoryList.h"

#include "Frette/Frette.h"
#include "Inventory/FretteInventoryComponent.h"

bool FFretteInventoryList::HasEntry(int32 ItemId) const
{
	return IdToIndexMap.Contains(ItemId);
}

UFretteInventoryItem* FFretteInventoryList::GetItemById(int32 ItemId) const
{
	if (const int32* Idx = GetIndexById(ItemId))
	{
		return Entries[*Idx].Item;
	}

	return nullptr;
}

void FFretteInventoryList::AddEntry(UFretteInventoryItem* ItemToAdd)
{
	check(Owner->GetOwner()->HasAuthority());
	require(IsValidItem(ItemToAdd, true));

	FFretteInventoryListEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = ItemToAdd;
	Entry.Item->Id = NextId++;
	IdToIndexMap.Add(Entry.Item->Id, Entries.Num() - 1);
	MarkItemDirty(Entry);

	Owner->OnItemAdded.Broadcast(Entry.Item);
	Owner->K2_OnItemAdded.Broadcast(Entry.Item);
}

void FFretteInventoryList::ChangeEntry(UFretteInventoryItem* ItemToChange)
{
	check(Owner->GetOwner()->HasAuthority());
	require(IsValidItem(ItemToChange));

	const int32* Idx = GetIndexById(ItemToChange->Id);
	require(Idx, "Inventory: Cannot change item #%d because it was not found in this inventory.", ItemToChange->Id);

	FFretteInventoryListEntry& Entry = Entries[*Idx];
	Entry.Item = ItemToChange;
	MarkItemDirty(Entry);

	Owner->OnItemChanged.Broadcast(Entry.Item);
	Owner->K2_OnItemChanged.Broadcast(Entry.Item);
}

void FFretteInventoryList::RemoveEntry(const UFretteInventoryItem* ItemToRemove)
{
	check(Owner.IsValid());
	check(Owner->GetOwner()->HasAuthority());

	require(Entries.Num() > 0, "Inventory: Cannot remove item because inventory is empty.");
	require(IsValidItem(ItemToRemove));

	const int32* IdxPtr = GetIndexById(ItemToRemove->Id);
	require(IdxPtr, "Inventory: Cannot remove item #%d because it was not found in this inventory.", ItemToRemove->Id);

	const int32 Idx = *IdxPtr;
	const int32 LastIdx = Entries.Num() - 1;
	const FFretteInventoryListEntry& EntryToRemove = Entries[Idx];

	if (Idx != LastIdx)
	{
		const FFretteInventoryListEntry& EntryToMove = Entries[LastIdx];
		IdToIndexMap[EntryToMove.Item->Id] = Idx;
		Entries.Swap(Idx, LastIdx);
	}

	IdToIndexMap.Remove(EntryToRemove.Item->Id);

	Owner->OnItemRemoved.Broadcast(EntryToRemove.Item);
	Owner->K2_OnItemRemoved.Broadcast(EntryToRemove.Item);

	Entries.RemoveAt(LastIdx);
	MarkArrayDirty();
}

bool FFretteInventoryList::IsValidItem(const UFretteInventoryItem* Item, bool bAllowInvalidId) const
{
	if (!IsValid(Item))
		return false;

	if (!bAllowInvalidId && !Item->HasValidID())
		return false;

	if (!IsValid(Item->Data))
		return false;

	if (Item->GetOuter() != Owner)
		return false; // not from the same inventory

	return true;
}

const int32* FFretteInventoryList::GetIndexById(int32 ItemId) const
{
	const int32* IdxPtr = IdToIndexMap.Find(ItemId);
	if (IdxPtr == nullptr)
		return nullptr;

	checkfSlow(Entries.IsValidIndex(*IdxPtr), TEXT("Inventory: Item ID maps to an out of bounds index."));
	checkfSlow(Entries[*IdxPtr].Item->Id == ItemId, TEXT("Inventory: Item ID maps to the index of a different item."));

	return IdxPtr;
}

bool FFretteInventoryList::HasValidOwner() const
{
	return Owner.IsValid() && Owner->GetOwner()->HasAuthority();
}

void FFretteInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FFretteInventoryListEntry& Entry = Entries[Index];
		IdToIndexMap.Add(Entry.Item->Id, Index);
		Owner->OnItemAdded.Broadcast(Entry.Item);
		Owner->K2_OnItemAdded.Broadcast(Entry.Item);
	}
}

void FFretteInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		const FFretteInventoryListEntry& Entry = Entries[Index];
		IdToIndexMap[Entry.Item->Id] = Index;
		Owner->OnItemChanged.Broadcast(Entry.Item);
		Owner->K2_OnItemChanged.Broadcast(Entry.Item);
	}
}

void FFretteInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		const FFretteInventoryListEntry& Entry = Entries[Index];
		IdToIndexMap.Remove(Entry.Item->Id);
		Owner->OnItemRemoved.Broadcast(Entry.Item);
		Owner->K2_OnItemRemoved.Broadcast(Entry.Item);
	}
}