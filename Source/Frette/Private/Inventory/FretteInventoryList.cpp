#include "Inventory/FretteInventoryList.h"

#include "Frette/Frette.h"
#include "Inventory/FretteInventoryComponent.h"

bool FFretteInventoryList::HasEntry(int32 ItemId) const
{
	return IdToIndexMap.Contains(ItemId);
}

UFretteInventoryItem* FFretteInventoryList::GetItemById(int32 ItemId) const
{
	if (const int32 Idx = GetIndexById(ItemId); Idx != INDEX_NONE)
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
	Entry.UpdatedIdx = INDEX_NONE; // clear in case it was set by a remove previously
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
		
		// notify the client to update the ID -> index map for the swapped item
		FFretteInventoryListEntry& MovedEntry = Entries[Idx];
		MovedEntry.UpdatedIdx = Idx;
		MarkItemDirty(MovedEntry);
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
		FRETTE_LOG(Log, "PostReplicatedAdd: Item ID %d at index %d", Entry.Item->Id, Index);
	}
}

void FFretteInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	check(Owner.IsValid());
	
	for (const int32 Index : ChangedIndices)
	{
		FFretteInventoryListEntry& Entry = Entries[Index];
		check(IsValidItem(Entry.Item));
		
		if (Entry.UpdatedIdx != INDEX_NONE)
		{
			IdToIndexMap[Entry.Item->Id] = Entry.UpdatedIdx;
			FRETTE_LOG(Log, "PostReplicatedChange: Item ID %d set to last removed index %d", Entry.Item->Id, Entry.UpdatedIdx);
			Entry.UpdatedIdx = INDEX_NONE; // update on client, server already updated
			return;
		}
		
		Owner->OnItemChanged.Broadcast(Entry.Item);
		Owner->K2_OnItemChanged.Broadcast(Entry.Item);
		FRETTE_LOG(Log, "PostReplicatedChange: Item ID %d at index %d", Entry.Item->Id, Index);
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
		FRETTE_LOG(Log, "PreReplicatedRemove: Item ID %d at index %d", Entry.Item->Id, Index);
	}
}

#if WITH_EDITOR
void UFretteInventoryComponent::DumpInventory()
{
	if (GetOwner()->HasAuthority())
	{
		Inventory.DumpInventory();
	}
	else
	{
		Inventory.DumpInventory();
		ServerDumpInventory();
	}
}

void FFretteInventoryList::DumpInventory() const
{
	if (!Owner.IsValid())
	{
		UE_LOG(LogFrette, Warning, TEXT("Inventory dump failed: Invalid owner"));
		return;
	}

	const AActor* OwnerActor = Owner->GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogFrette, Warning, TEXT("Inventory dump failed: Invalid owner actor"));
		return;
	}

	const bool bIsServer = OwnerActor->HasAuthority();
	const TCHAR* RoleStr = bIsServer ? TEXT("SERVER") : TEXT("CLIENT");

	TStringBuilder<2048> Builder;

	Builder.Appendf(TEXT("Inventory dump on %s for: %s\n"), RoleStr, *OwnerActor->GetName());

	Builder.Appendf(TEXT("=== %d inventory entries ===\n"), Entries.Num());
	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		const FFretteInventoryListEntry& Entry = Entries[i];
		if (IsValid(Entry.Item))
		{
			const FString DataName = IsValid(Entry.Item->Data) ? Entry.Item->Data->GetName() : TEXT("NULL");
			const FString DisplayName = IsValid(Entry.Item->Data) ? Entry.Item->Data->DisplayName.ToString() : TEXT("N/A");
			Builder.Appendf(TEXT("  [%d] Item ID: %d | Data: %s (%s) | UpdatedIdx: %d\n"),
				i, Entry.Item->Id, *DataName, *DisplayName, Entry.UpdatedIdx);
		}
		else
		{
			Builder.Appendf(TEXT("  [%d] Item: NULL | RepID: %d | RepKey: %d\n"),
				i, Entry.ReplicationID, Entry.ReplicationKey);
		}
	}

	Builder.Appendf(TEXT("\n=== %d ID to index mappings ===\n"), IdToIndexMap.Num());
	TArray<int32> Keys;
	IdToIndexMap.GetKeys(Keys);
	Keys.Sort();

	bool bHasErrors = false;
	TStringBuilder<512> ErrorBuilder;

	for (const int32 ItemId : Keys)
	{
		const int32 MappedIndex = IdToIndexMap[ItemId];
		const bool bValidIndex = Entries.IsValidIndex(MappedIndex);
		const bool bConsistent = bValidIndex && Entries[MappedIndex].Item && Entries[MappedIndex].Item->Id == ItemId;

		Builder.Appendf(TEXT("  ItemID %d -> Index %d [%s]\n"), ItemId, MappedIndex, bConsistent ? TEXT("OK") : TEXT("ERROR"));

		if (!bConsistent)
		{
			bHasErrors = true;
			if (!bValidIndex)
				ErrorBuilder.Appendf(TEXT("    ERROR: ItemID %d -> Index %d is out of bounds (Num=%d)\n"), ItemId, MappedIndex, Entries.Num());
			else if (!Entries[MappedIndex].Item)
				ErrorBuilder.Appendf(TEXT("    ERROR: ItemID %d -> Index %d has NULL item\n"), ItemId, MappedIndex);
			else
				ErrorBuilder.Appendf(TEXT("    ERROR: ItemID %d -> Index %d has different ID (%d)\n"), ItemId, MappedIndex, Entries[MappedIndex].Item->Id);
		}
	}

	Builder.Append(TEXT("\n\n"));

	UE_LOG(LogFrette, Log, TEXT("%s"), Builder.ToString());

	if (bHasErrors)
		UE_LOG(LogFrette, Error, TEXT("%s"), ErrorBuilder.ToString());
}

void UFretteInventoryComponent::ServerDumpInventory_Implementation()
{
	Inventory.DumpInventory();
}

static void ExecDumpInventory(UWorld* World)
{
	require(World);

	const APlayerController* PC = World->GetFirstPlayerController();
	require(PC);

	const APawn* Pawn = PC->GetPawn();
	require(Pawn);

	UFretteInventoryComponent* InventoryComp = Pawn->FindComponentByClass<UFretteInventoryComponent>();
	require(InventoryComp, "DumpInventory: No inventory component found on pawn %s", *Pawn->GetName());

	InventoryComp->DumpInventory();
}

static FAutoConsoleCommandWithWorld FCmdDumpInventory(
	TEXT("Frette.DumpInventory"),
	TEXT("Dumps the inventory contents of the local player's pawn on both server and client. Usage: Frette.DumpInventory"),
	FConsoleCommandWithWorldDelegate::CreateStatic(ExecDumpInventory)
);
#endif // WITH_EDITOR
