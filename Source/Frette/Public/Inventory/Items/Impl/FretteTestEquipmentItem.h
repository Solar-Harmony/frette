#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/FretteGearItem.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "FretteTestEquipmentItem.generated.h"

UCLASS(BlueprintType)
class UFretteTestEquipmentItem : public UFretteGearItem
{
	GENERATED_BODY()

public:
	virtual void Use() override {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	int Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	int CurrentAmmo;
};

UCLASS()
class UFretteTestEquipmentItemDataAsset : public UFretteInventoryItemDataAsset
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

	virtual UFretteInventoryItem* CreateRuntimeItem(UObject* Outer) override
	{
		UFretteTestEquipmentItem* Item = NewObject<UFretteTestEquipmentItem>(Outer);
		Item->Data = this;
		Item->Durability = MaxDurability;
		Item->CurrentAmmo = MagazineCapacity;
		return Item;
	}
};