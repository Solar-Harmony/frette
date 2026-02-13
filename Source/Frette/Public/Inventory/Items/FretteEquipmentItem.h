#pragma once

#include "FretteInventoryItem.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "FretteEquipmentItem.generated.h"

// An equippable item that supports arbitrary positioning, with no stacking support.
// Use for unique items that require per-item data like durability, ammo count, etc.
UCLASS(BlueprintType)
class UFretteEquipmentItem : public UFretteInventoryItem
{
	GENERATED_BODY()

public:
	// Index of the visual inventory slot the item is in, -1 if unassigned. 
	// This is managed by the UI widget and may differ from the item's index in the replicated inventory array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 SlotIndex = -1;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, SlotIndex);
	}
};

UCLASS(BlueprintType, Category = "Frette❄️|Inventory")
class UFretteEquipmentItemDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	// Tag to restrict which inventory slots the item can be placed in.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (Categories = "Frette.EquipmentSlot"))
	FGameplayTag SlotTag = FGameplayTag();

	virtual UFretteInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		auto* Item = NewObject<UFretteEquipmentItem>(Outer);
		Item->Data = this;
		return Item;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, SlotTag);
	}
};