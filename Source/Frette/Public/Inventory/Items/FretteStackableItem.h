#pragma once

#include "Inventory/Items/FretteInventoryItem.h"
#include "FretteStackableItem.generated.h"

// An item with a stack size but no other runtime properties.
// Use for stacks of identical items, like crafting materials, ammo, etc.
UCLASS(BlueprintType)
class UFretteStackableItem final : public UFretteInventoryItem
{
	GENERATED_BODY()

public:
	FRETTE_ITEM_DATA_GETTER(UFretteStackableItemDataAsset)
	
	UFUNCTION(BlueprintCallable)
	int UpdateQuantity(int NumToAdd);
	
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
	UPROPERTY(EditDefaultsOnly)
	int DefaultQuantity = 1;
	
	UPROPERTY(EditDefaultsOnly)
	int MaxStackSize = 64;
	
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteStackableItem::StaticClass(); }
	
protected:
	virtual void InitializeItem(UFretteInventoryItem* Item) override
	{
		Super::InitializeItem(Item);
		
		UFretteStackableItem* Stack = Cast<UFretteStackableItem>(Item);
		Stack->Quantity = DefaultQuantity;
	}
};

inline int UFretteStackableItem::UpdateQuantity(int NumToAdd)
{
	const int OldQuantity = Quantity;
	
	Quantity += NumToAdd;
	Quantity = FMath::Clamp(Quantity, 0, GetData()->MaxStackSize);
	
	// return how many bullets we were able to add or remove
	return Quantity - OldQuantity; 
}
