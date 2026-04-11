#pragma once

#include "FretteInventoryItem.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySystem/FretteAbilitySetDataAsset.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "FretteSlottableItem.generated.h"

class UFretteSlottableItemDataAsset;

UCLASS(BlueprintType)
class UFretteSlottableItem : public UFretteInventoryItem
{
	GENERATED_BODY()

public:
	FRETTE_ITEM_DATA_GETTER(UFretteSlottableItemDataAsset)

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 SlotID = -1;
	
	virtual void OnEquipped();
	virtual void OnUnequipped();


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, SlotID);
	}
};

UCLASS(BlueprintType, Category = "Frette|Inventory")
class UFretteSlottableItemDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Frette.BodyPart"))
	FGameplayTag ItemSlotTag;

	UPROPERTY(EditDefaultsOnly, DisplayName="Abilities Granted on Equip")
	FFretteAbilitySet GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, DisplayName="Effects Applied on Equip")
	TArray<FFretteGameplayEffectConfig> GrantedEffects;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> AnimLayer;

	virtual const UClass* GetRuntimeItemClass() const override { return UFretteSlottableItem::StaticClass(); }
};