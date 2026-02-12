#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItemDataAsset.generated.h"

class UInventoryItemDataAsset;

UCLASS(BlueprintType, Blueprintable)
class UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UInventoryItemDataAsset* Data = nullptr;

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, Data);
	}
};

// An item with no runtime data, but a stack size.
UCLASS(BlueprintType)
class UInventoryStack final : public UInventoryItem
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

static const FPrimaryAssetType GInventoryItemPrimaryAssetType("Item");

UCLASS(Abstract, BlueprintType)
class UInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	virtual UInventoryItem* CreateRuntimeItem(UObject* Outer)
	{
		UInventoryItem* Item = NewObject<UInventoryItem>(Outer);
		Item->Data = this;
		return Item;
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
};

UCLASS(BlueprintType)
class UInventoryStackDataAsset : public UInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackSize = 64;

	virtual UInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		UInventoryStack* Item = NewObject<UInventoryStack>(Outer);
		Item->Data = this;
		Item->Quantity = 1;
		return Item;
	}
};