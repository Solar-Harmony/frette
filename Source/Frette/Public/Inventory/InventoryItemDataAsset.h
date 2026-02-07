#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryItemDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInventoryItemDataAsset> Data = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponInventoryItem final : public FInventoryItem
{
	GENERATED_BODY()

	FWeaponInventoryItem() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAmmo;
};

static const FPrimaryAssetType GInventoryItemPrimaryAssetType("Item");

UCLASS(BlueprintType)
class UInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetRuntimeDataType() const { return FInventoryItem::StaticStruct(); }

	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, meta=(MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
};

UCLASS()
class UTestWeaponInventoryItemDataAsset : public UInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetRuntimeDataType() const override { return FWeaponInventoryItem::StaticStruct(); }

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	int MaxDurability = 100;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	int BaseDamage = 10;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, Units = "cm"))
	float MaxSpreadRadiusCm = 5.0f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	int MagazineCapacity = 30;
};