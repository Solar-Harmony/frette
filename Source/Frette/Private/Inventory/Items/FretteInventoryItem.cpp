#pragma once

#include "Inventory/Items/FretteInventoryItem.h"

#include "Character/FretteBaseCharacter.h"
#include "Inventory/FretteInventoryComponent.h"

UFretteInventoryComponent* UFretteInventoryItem::GetOwningInventory() const
{
	return Cast<UFretteInventoryComponent>(GetOuter());
}

// TODO: Will not work if the item is owned by a chest etc.
// we should use interfaces instead of base classes
AFretteBaseCharacter* UFretteInventoryItem::GetOwningPlayer() const
{
	return Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());
}

void UFretteInventoryItem::Use()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());
	if (!Character->HasAuthority())
		return;

	if (UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character))
	{
		for (const FFretteGameplayEffectConfig& EffectConfig : Data->EffectsAppliedOnUse)
		{
			ASC->ApplyEffect(EffectConfig, this);
		}
	}
	
	Use_Implementation();
}