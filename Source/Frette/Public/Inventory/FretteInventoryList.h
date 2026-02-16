#pragma once

#include "CoreMinimal.h"
#include "Items/FretteInventoryItem.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "FretteInventoryList.generated.h"

class UFretteInventoryComponent;
struct FFretteInventoryList;

// Wrapper around a TArray with delta serialization support.
// This is necessary for replicating parts of the array at a time instead of the whole thing every modification,
// and notify us of changes to individual items so we can update the UI.
USTRUCT()
struct FFretteInventoryListEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	UFretteInventoryItem* Item = nullptr;
};

USTRUCT()
struct FFretteInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FFretteInventoryList() = default;

	explicit FFretteInventoryList(UFretteInventoryComponent* InOwner)
		: Owner(InOwner) {}

	int32 Num() const { return Entries.Num(); }
	UFretteInventoryItem* GetItemById(int32 ItemId) const;
	void AddEntry(UFretteInventoryItem* ItemToAdd);
	void ChangeEntry(UFretteInventoryItem* ItemToChange);
	void RemoveEntry(const UFretteInventoryItem* ItemToRemove);
	bool HasValidItemData(const UFretteInventoryItem* Item) const;

	//~FFastArraySerializer contract
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FFretteInventoryListEntry, FFretteInventoryList>(Entries, DeltaParms, *this);
	}

private:
	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UFretteInventoryComponent> Owner = nullptr;

	UPROPERTY(Transient)
	TArray<FFretteInventoryListEntry> Entries;

	int32 NextId = 1;
	constexpr static int32 InvalidID = 0;
	TMap<int32, int32> IdToIndexMap;

	int32 GetIndexByIdChecked(int32 ItemId) const;
};

template <> struct TStructOpsTypeTraits<FFretteInventoryList> : public TStructOpsTypeTraitsBase2<FFretteInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};