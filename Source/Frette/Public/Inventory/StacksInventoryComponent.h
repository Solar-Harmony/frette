#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryComponent.h"

#include "StacksInventoryComponent.generated.h"

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent), Category="Inventory")
class FRETTE_API UStacksInventoryComponent : public UActorComponent, public IInventoryComponent
{
	GENERATED_BODY()

public:
	UInventoryItem* GetItem(UInventoryItemDataAsset* ItemType);
	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) override;

	UInventoryItem* AddItem(UInventoryItemDataAsset* ItemType, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable)
	UInventoryItem* K2_AddItem(UInventoryItemDataAsset* ItemType, int32 Quantity = 1) { return AddItem(ItemType, Quantity); }

	bool RemoveItem(UInventoryItemDataAsset* ItemType, int32 Quantity = 1);

private:
	UPROPERTY()
	TMap<FPrimaryAssetId, TObjectPtr<UInventoryItem>> Items;
};