#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryComponent.h"

#include "StacksInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FStackedInventoryItem final : public FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity;
};

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent), Category="Inventory")
class FRETTE_API UStacksInventoryComponent : public UActorComponent, public IInventoryComponent
{
	GENERATED_BODY()

public:
	FStackedInventoryItem& GetOrAddItem(UInventoryItemDataAsset* ItemType);
	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) override;

	FStackedInventoryItem& AddItem(UInventoryItemDataAsset* ItemType, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable)
	FStackedInventoryItem K2_AddItem(UInventoryItemDataAsset* ItemType, int32 Quantity = 1) { return AddItem(ItemType, Quantity); }

	bool RemoveItem(UInventoryItemDataAsset* ItemType, int32 Quantity = 1);

private:
	TMap<FPrimaryAssetId, FStackedInventoryItem> Items;
};