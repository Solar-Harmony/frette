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

	UFretteInventoryItem* GetEntry(int32 Index) const;
	int32 Num() const { return Entries.Num(); }
	void AddEntry(UFretteInventoryItem* ItemToAdd);
	void ChangeEntry(UFretteInventoryItem* ItemToChange);
	void RemoveEntry(const UFretteInventoryItem* ItemToRemove);
	bool IsValidItem(const UFretteInventoryItem* Item) const;
	int32 GetItemIndexChecked(const UFretteInventoryItem* Item) const;

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

	TMap<uint64, int32> IdToIndexMap;
	uint64 NextId = 1;
	constexpr static uint64 InvalidID = 0;
};

template <> struct TStructOpsTypeTraits<FFretteInventoryList> : public TStructOpsTypeTraitsBase2<FFretteInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};