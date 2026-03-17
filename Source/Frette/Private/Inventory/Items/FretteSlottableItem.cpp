#include "Inventory/Items/FretteSlottableItem.h"

#include "Character/FretteBaseCharacter.h"
#include "Inventory/FretteInventoryComponent.h"

void UFretteSlottableItem::OnEquipped()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());

	if (UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character))
	{
		ASC->GrantAbilitiesFromAbilitySet(GetData()->GrantedAbilities, this);
		
		for (const FFretteGameplayEffectConfig& EffectConfig : GetData()->GrantedEffects)
		{
			ASC->ApplyEffect(EffectConfig, this);
		}
	}
}

void UFretteSlottableItem::OnUnequipped()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());

	if (UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character))
	{
		ASC->RemoveAbilitiesFromAbilitySet(GetData()->GrantedAbilities);
		
		for (const FFretteGameplayEffectConfig& EffectConfig : GetData()->GrantedEffects)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(EffectConfig.EffectClass, ASC, 1);
		}
	}
}