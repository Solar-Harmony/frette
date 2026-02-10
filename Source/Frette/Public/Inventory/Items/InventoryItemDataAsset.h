#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItemDataAsset.generated.h"

class UInventoryItemDataAsset;

UCLASS(BlueprintType, EditInlineNew)
class UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TObjectPtr<UInventoryItemDataAsset> Data = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Quantity = 1;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(UInventoryItem, Data);
		DOREPLIFETIME(UInventoryItem, Quantity);
	}

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