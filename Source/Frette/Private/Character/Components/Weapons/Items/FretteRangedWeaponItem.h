#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/FretteStackableItem.h"
#include "Inventory/Items/Impl/FretteWeaponItem.h"
#include "FretteRangedWeaponItem.generated.h"

class AFretteProjectile;
class UFretteRangedWeaponDataAsset;

UCLASS()
class UFretteRangedWeaponItem : public UFretteWeaponItem
{
	GENERATED_BODY()

public:
	FRETTE_ITEM_DATA_GETTER(UFretteRangedWeaponDataAsset)

	int GetCurrentAmmo() const { return NumBulletsLoaded; }
	bool TryUseAmmo();
	
	UFUNCTION(BlueprintCallable)
	int Reload();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME_CONDITION(UFretteRangedWeaponItem, NumBulletsLoaded, COND_OwnerOnly);
	}

private:
	UPROPERTY(Replicated)
	int NumBulletsLoaded;
};

UCLASS()
class UFretteRangedWeaponDataAsset : public UFretteWeaponItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int MaxClipAmmo = 30;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve DistanceDamageFalloff;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteStackableItemDataAsset> AmmoType;

	UPROPERTY(EDitDefaultsOnly)
	TSubclassOf<AFretteProjectile> ProjectileType;

	UPROPERTY(EditDefaultsOnly)
	FTransform ProjectileSpawnTransform;

private:
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteRangedWeaponItem::StaticClass(); }

	virtual void InitializeItem(UFretteInventoryItem* Item) override
	{
		Super::InitializeItem(Item);

		UFretteRangedWeaponItem* RangedWeaponItem = Cast<UFretteRangedWeaponItem>(Item);
		check(RangedWeaponItem);
		RangedWeaponItem->Reload();
	}
};