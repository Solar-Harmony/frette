#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/UnrealNetwork.h"
#include "FretteInventoryItem.generated.h"

class UFretteInventoryItemDataAsset;

static const FPrimaryAssetType GInventoryItemPrimaryAssetType("FretteInventoryItem");

UCLASS(Abstract, BlueprintType, Blueprintable)
class UFretteInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 Id;

	UPROPERTY(BlueprintReadOnly, Replicated)
	UFretteInventoryItemDataAsset* Data = nullptr;

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, Data);
		DOREPLIFETIME(ThisClass, Id);
	}
};

UCLASS(Abstract, BlueprintType)
class UFretteInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	virtual UFretteInventoryItem* CreateRuntimeItem(UObject* Outer)
	{
		auto* Item = NewObject<UFretteInventoryItem>(Outer);
		Item->Data = this;
		return Item;
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
};