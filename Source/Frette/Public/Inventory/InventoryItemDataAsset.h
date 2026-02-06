#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InventoryItemDataAsset.generated.h"

static const FPrimaryAssetType GInventoryItemPrimaryAssetType("Item");

UCLASS()
class UInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	// In which section of the UI we should display the item.
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Inventory.Section"))
	FGameplayTag Section;

	// If the item's Section is a slot container, defines the space it takes. Ignored otherwise.
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1))
	int SlotWidth = 1;

	// If the item's Section is a slot container, defines the space it takes. Ignored otherwise.
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1))
	int SlotHeight = 1;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
};