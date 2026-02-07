#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryItemDataAsset.generated.h"

class UInventoryItemDataAsset;

UCLASS(BlueprintType, Blueprintable)
class UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	TObjectPtr<UInventoryItemDataAsset> Data = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 Quantity = 1;

	virtual bool IsSupportedForNetworking() const override { return true; }
};

static const FPrimaryAssetType GInventoryItemPrimaryAssetType("Item");

UCLASS(BlueprintType)
class UInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, meta=(MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	// TODO: not used by stacking
	virtual UInventoryItem* CreateRuntimeItem(UObject* Outer)
	{
		UInventoryItem* Item = NewObject<UInventoryItem>(Outer);
		Item->Data = this;
		Item->Quantity = 0;
		return Item;
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
};