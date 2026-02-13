#pragma once

#include "FretteInventoryItem.h"
#include "GameplayTagContainer.h"
#include "FretteSlottedItem.generated.h"

// An equippable item that supports arbitrary positioning, with no stacking support.
// Use for unique items that require per-item data like durability, ammo count, etc.
UCLASS(BlueprintType)
class UFretteSlottedItem : public UFretteInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bEquipped = false;

	// Index of the visual inventory slot the item is in, -1 if unassigned. 
	// This is managed by the UI widget and may differ from the item's index in the replicated inventory array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 SlotIndex = -1;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, SlotIndex);
		DOREPLIFETIME(ThisClass, bEquipped);
	}
};

UCLASS(BlueprintType, Category = "Frette❄️|Inventory")
class USlottedItemDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	// Tag to restrict which inventory slots the item can be placed in.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FGameplayTag SlotTag = FGameplayTag();

	virtual UFretteInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		auto* Item = NewObject<UFretteSlottedItem>(Outer);
		Item->Data = this;
		return Item;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, SlotTag);
	}
};