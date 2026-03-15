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
	
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteStackableItem::StaticClass(); }
};