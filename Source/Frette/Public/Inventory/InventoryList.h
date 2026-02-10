#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryList.generated.h"

struct FInventoryList;

USTRUCT()
struct FInventoryListEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	UInventoryItem* Item = nullptr;
};

USTRUCT()
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FInventoryList() = default;

	explicit FInventoryList(UActorComponent* InOwner)
		: Owner(InOwner) {}

	TArray<UInventoryItem*> GetAllItems() const;
	void AddEntry(UInventoryItem* Instance);
	UInventoryItem* AddEntry(UInventoryItemDataAsset* ItemClass);
	// TODO: Remove entry

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FInventoryListEntry, FInventoryList>(Entries, DeltaParms, *this);
	}

	UPROPERTY()
	TArray<FInventoryListEntry> Entries;

private:
	UPROPERTY(NotReplicated)
	UActorComponent* Owner = nullptr;
};

template <> struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};