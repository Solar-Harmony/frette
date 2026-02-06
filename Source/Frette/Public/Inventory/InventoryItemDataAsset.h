#pragma once

#include "CoreMinimal.h"
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
	int32 Area;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
};