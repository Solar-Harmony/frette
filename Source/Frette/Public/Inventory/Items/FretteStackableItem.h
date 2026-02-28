#pragma once

#include "FretteInventoryItem.h"
#include "FretteStackableItem.generated.h"

// An item with a stack size but no other runtime properties.
// Use for stacks of identical items, like crafting materials, ammo, etc.
UCLASS(BlueprintType)
class UFretteStackableItem final : public UFretteInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Quantity = 1;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, Quantity);
	}
};

UCLASS(BlueprintType, Category = "Frette|Inventory")
class UFretteStackableItemDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	virtual UFretteInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		auto* Item = NewObject<UFretteStackableItem>(Outer);
		Item->Data = this;
		return Item;
	}
};