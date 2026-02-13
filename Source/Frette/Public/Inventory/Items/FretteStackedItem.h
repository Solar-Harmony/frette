#pragma once

#include "FretteInventoryItem.h"
#include "FretteStackedItem.generated.h"

// An item with a stack size but no other runtime properties.
// Use for stacks of identical items, like crafting materials, ammo, etc.
UCLASS(BlueprintType)
class UFretteStackedItem final : public UFretteInventoryItem
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

UCLASS(BlueprintType, Category = "Frette❄️|Inventory")
class UFretteInventoryStackDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	virtual UFretteInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		auto* Item = NewObject<UFretteStackedItem>(Outer);
		Item->Data = this;
		Item->Quantity = 1;
		return Item;
	}
};