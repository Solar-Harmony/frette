#include "Inventory/Items/FretteSlottableItem.h"

#include "Character/FretteBaseCharacter.h"
#include "Inventory/FretteInventoryComponent.h"

void UFretteSlottableItem::OnEquipped()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());
	UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character); 
	const UFretteSlottableItemDataAsset* ItemData = Cast<UFretteSlottableItemDataAsset>(Data);
	
	if (ASC)
	{
		ASC->GrantAbilitiesFromAbilitySet(ItemData->GrantedAbilities, this);
		
		for (const FFretteGameplayEffectConfig& EffectConfig : ItemData->GrantedEffects)
		{
			ASC->ApplyEffect(EffectConfig, this);
		}
	}
}

void UFretteSlottableItem::OnUnequipped()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());
	UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character); 
	const UFretteSlottableItemDataAsset* ItemData = Cast<UFretteSlottableItemDataAsset>(Data);
	
	if (ASC)
	{
		ASC->RemoveAbilitiesFromAbilitySet(ItemData->GrantedAbilities);
		
		for (const FFretteGameplayEffectConfig& EffectConfig : ItemData->GrantedEffects)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(EffectConfig.EffectClass, ASC, 1);
		}
	}
}