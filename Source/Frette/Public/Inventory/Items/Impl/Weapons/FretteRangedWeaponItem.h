#pragma once

#include "CoreMinimal.h"
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
	
	int GetCurrentAmmo() const { return CurrentClipAmmo; }
	void UseAmmo();
	void Reload();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME_CONDITION(UFretteRangedWeaponItem, CurrentClipAmmo, COND_OwnerOnly);
	}
	
private:
	UPROPERTY(Replicated)
	int CurrentClipAmmo;
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
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AFretteProjectile> ProjectileType;
	
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