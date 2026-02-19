#include "Inventory/FretteInventoryList.h"

#include "Frette/Frette.h"
#include "Inventory/FretteInventoryComponent.h"

bool FFretteInventoryList::HasEntry(int32 ItemId) const
{
	return IdToIndexMap.Contains(ItemId);
}

UFretteInventoryItem* FFretteInventoryList::GetItemById(int32 ItemId) const
{
	const int32 Idx = GetIndexById(ItemId);
	
	if (Idx != INDEX_NONE)
	{
		return Entries[Idx].Item;
	}

	return nullptr;
}

void FFretteInventoryList::AddEntry(UFretteInventoryItem* ItemToAdd)
{
	check(Owner.IsValid());
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
	check(Owner.IsValid());
	check(Owner->GetOwner()->HasAuthority());
	require(IsValidItem(ItemToChange));

	const int32 Idx = GetIndexById(ItemToChange->Id);
	require(Idx != INDEX_NONE, "Inventory: Cannot change item #%d because it was not found in this inventory.", ItemToChange->Id);

	FFretteInventoryListEntry& Entry = Entries[Idx];
	Entry.Item = ItemToChange;
	MarkItemDirty(Entry);

	Owner->OnItemChanged.Broadcast(Entry.Item);
	Owner->K2_OnItemChanged.Broadcast(Entry.Item);
}

void FFretteInventoryList::RemoveEntry(int32 ItemId)
{
	check(Owner.IsValid());
	check(Owner->GetOwner()->HasAuthority());
	require(Entries.Num() > 0, "Inventory: Cannot remove item because inventory is empty.");

	const int32 Idx = GetIndexById(ItemId);
	require(Idx != INDEX_NONE, "Inventory: Cannot remove item #%d because it was not found in this inventory.", ItemId);
	
	const int32 LastIdx = Entries.Num() - 1;
	UFretteInventoryItem* ItemToRemove = Entries[Idx].Item;

	if (Idx != LastIdx)
	{
		const int32 IdToMove = Entries[LastIdx].Item->Id;
		IdToIndexMap[IdToMove] = Idx;
		Entries.Swap(Idx, LastIdx);
		MarkItemDirty(Entries[Idx]);
	}

	Owner->RemoveReplicatedSubObject(ItemToRemove);
	Owner->OnItemRemoved.Broadcast(ItemToRemove);
	Owner->K2_OnItemRemoved.Broadcast(ItemToRemove);
	
	IdToIndexMap.Remove(ItemToRemove->Id);
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

int32 FFretteInventoryList::GetIndexById(int32 ItemId) const
{
	if (!IdToIndexMap.Contains(ItemId))
		return INDEX_NONE;
	
	const int32 Idx = IdToIndexMap[ItemId];

	checkfSlow(Entries.IsValidIndex(Idx), TEXT("Inventory: Item ID maps to an out of bounds index."));
	checkfSlow(Entries[Idx].Item->Id == ItemId, TEXT("Inventory: Item ID maps to the index of a different item."));

	return Idx;
}

bool FFretteInventoryList::HasValidOwner() const
{
	return Owner.IsValid() && Owner->GetOwner()->HasAuthority();
}

void FFretteInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	check(Owner.IsValid());
	
	for (const int32 Index : AddedIndices)
	{
		const FFretteInventoryListEntry& Entry = Entries[Index];
		check(IsValidItem(Entry.Item));
		IdToIndexMap.Add(Entry.Item->Id, Index);
		Owner->OnItemAdded.Broadcast(Entry.Item);
		Owner->K2_OnItemAdded.Broadcast(Entry.Item);
	}
}

void FFretteInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	check(Owner.IsValid());
	
	for (const int32 Index : ChangedIndices)
	{
		const FFretteInventoryListEntry& Entry = Entries[Index];
		check(IsValidItem(Entry.Item));
		IdToIndexMap[Entry.Item->Id] = Index;
		Owner->OnItemChanged.Broadcast(Entry.Item);
		Owner->K2_OnItemChanged.Broadcast(Entry.Item);
	}
}

void FFretteInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	check(Owner.IsValid());
	
	for (const int32 Index : RemovedIndices)
	{
		const FFretteInventoryListEntry& Entry = Entries[Index];
		check(IsValidItem(Entry.Item));
		IdToIndexMap.Remove(Entry.Item->Id);
		Owner->OnItemRemoved.Broadcast(Entry.Item);
		Owner->K2_OnItemRemoved.Broadcast(Entry.Item);
	}
}

void FFretteInventoryList::DumpInventory() const
{
	if (!Owner.IsValid())
	{
		UE_LOG(LogFrette, Warning, TEXT("Inventory dump failed: Invalid owner"));
		return;
	}

	AActor* OwnerActor = Owner->GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogFrette, Warning, TEXT("Inventory dump failed: Invalid owner actor"));
		return;
	}

	const bool bIsServer = OwnerActor->HasAuthority();
	const FString RoleStr = bIsServer ? TEXT("SERVER") : TEXT("CLIENT");

	UE_LOG(LogFrette, Log, TEXT("======================================"));
	UE_LOG(LogFrette, Log, TEXT("Inventory Dump [%s] for: %s"), *RoleStr, *OwnerActor->GetName());
	UE_LOG(LogFrette, Log, TEXT("======================================"));
	UE_LOG(LogFrette, Log, TEXT("Total Items: %d"), Entries.Num());
	UE_LOG(LogFrette, Log, TEXT("Next ID: %d"), NextId);
	UE_LOG(LogFrette, Log, TEXT(""));

	UE_LOG(LogFrette, Log, TEXT("--- Inventory Entries ---"));
	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		const FFretteInventoryListEntry& Entry = Entries[i];
		if (IsValid(Entry.Item))
		{
			const FString DataName = IsValid(Entry.Item->Data) ? Entry.Item->Data->GetName() : TEXT("NULL");
			const FString DisplayName = IsValid(Entry.Item->Data) ? Entry.Item->Data->DisplayName.ToString() : TEXT("N/A");
			UE_LOG(LogFrette, Log, TEXT("  [%d] Item ID: %d | Data: %s (%s) | RepID: %d | RepKey: %d"), 
				i, 
				Entry.Item->Id, 
				*DataName,
				*DisplayName,
				Entry.ReplicationID,
				Entry.ReplicationKey);
		}
		else
		{
			UE_LOG(LogFrette, Log, TEXT("  [%d] Item: NULL | RepID: %d | RepKey: %d"), 
				i,
				Entry.ReplicationID,
				Entry.ReplicationKey);
		}
	}

	UE_LOG(LogFrette, Log, TEXT(""));
	UE_LOG(LogFrette, Log, TEXT("--- ID to Index Map ---"));
	UE_LOG(LogFrette, Log, TEXT("Map Size: %d"), IdToIndexMap.Num());
	
	TArray<int32> Keys;
	IdToIndexMap.GetKeys(Keys);
	Keys.Sort();

	for (const int32 ItemId : Keys)
	{
		const int32 MappedIndex = IdToIndexMap[ItemId];
		const bool bValidIndex = Entries.IsValidIndex(MappedIndex);
		const bool bConsistent = bValidIndex && Entries[MappedIndex].Item && Entries[MappedIndex].Item->Id == ItemId;
		const FString ConsistencyStr = bConsistent ? TEXT("OK") : TEXT("ERROR");
		
		UE_LOG(LogFrette, Log, TEXT("  ItemID %d -> Index %d [%s]"), ItemId, MappedIndex, *ConsistencyStr);
		
		if (!bConsistent)
		{
			if (!bValidIndex)
			{
				UE_LOG(LogFrette, Error, TEXT("    ERROR: Index %d is out of bounds (Num=%d)"), MappedIndex, Entries.Num());
			}
			else if (!Entries[MappedIndex].Item)
			{
				UE_LOG(LogFrette, Error, TEXT("    ERROR: Entry at index %d has NULL item"), MappedIndex);
			}
			else
			{
				UE_LOG(LogFrette, Error, TEXT("    ERROR: Entry at index %d has different ID (%d)"), MappedIndex, Entries[MappedIndex].Item->Id);
			}
		}
	}

	UE_LOG(LogFrette, Log, TEXT("======================================"));
}