#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDataAsset.h"
#include "TestWeaponInventoryItemDataAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UWeaponInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAmmo;
};

UCLASS()
class UTestWeaponInventoryItemDataAsset : public UInventoryItemDataAsset
{
	GENERATED_BODY()

public:
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

	virtual UInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		UWeaponInventoryItem* Item = NewObject<UWeaponInventoryItem>(Outer);
		Item->Data = this;
		Item->Durability = MaxDurability;
		Item->CurrentAmmo = MagazineCapacity;
		return Item;
	}
};