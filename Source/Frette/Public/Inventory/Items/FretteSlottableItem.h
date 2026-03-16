#pragma once

#include "AbilitySystemBlueprintLibrary.h"
#include "FretteInventoryItem.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySystem/AbilitySetDataAsset.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "FretteSlottableItem.generated.h"


UCLASS(BlueprintType)
class UFretteSlottableItem : public UFretteInventoryItem
{
	GENERATED_BODY()

public:
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
class UFretteSlottableDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Frette.Inventory.SlotType"))
	FGameplayTag ItemSlotTag;
	
	UPROPERTY(EditDefaultsOnly)
	UAbilitySetDataAsset* GrantedAbilities;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FFretteGameplayEffectConfig> GrantedEffects;
	
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteSlottableItem::StaticClass(); }
};