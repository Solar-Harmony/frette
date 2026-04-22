#pragma once
#include "Inventory/Items/FretteSlottableItem.h"
#include "FretteClothingItem.generated.h"

class UFretteTemperatureComponent;
class UFretteClotheItemDataAsset;

UCLASS(BlueprintType)
class UFretteClothingItem : public UFretteSlottableItem
{
	GENERATED_BODY()
	FRETTE_ITEM_DATA_GETTER(UFretteClotheItemDataAsset)

public:
	UFUNCTION()
	virtual void OnEquipped() override;

	UFUNCTION()
	virtual void OnUnequipped() override;

	void UpdateBodyPartTemperatureModifier(bool bIsEquipping) const;
};

UCLASS(BlueprintType)
class UFretteClotheItemDataAsset : public UFretteSlottableItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMax = "1.0"))
	float ThermalImpedanceModifier = 0;

	virtual const UClass* GetRuntimeItemClass() const override { return UFretteClothingItem::StaticClass(); }

};