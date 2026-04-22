#include "Inventory/Items/FretteSlottableItem.h"

#include "Character/FretteBaseCharacter.h"
#include "Inventory/FretteInventoryComponent.h"

void UFretteSlottableItem::OnEquipped()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());

	if (USkeletalMeshComponent* Mesh = Character->GetMesh())
	{
		Mesh->LinkAnimClassLayers(GetData()->AnimLayer);
	}

	if (!Character->HasAuthority())
		return;

	if (UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character))
	{
		ASC->GrantAbilities(GetData()->GrantedAbilities.GetAbilityConfigs(), this);

		for (const FFretteGameplayEffectConfig& EffectConfig : GetData()->GrantedEffects)
		{
			ASC->ApplyEffect(EffectConfig, this);
		}
	}
}

void UFretteSlottableItem::OnUnequipped()
{
	AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());

	if (USkeletalMeshComponent* Mesh = Character->GetMesh())
	{
		//TODO: Devra link un layer de unnarmed
		Mesh->LinkAnimClassLayers(GetData()->AnimLayer);
	}

	if (!Character->HasAuthority())
		return;

	if (UFretteAbilitySystemComponent* ASC = UFretteAbilitySystemComponent::Get(Character))
	{
		ASC->RevokeAbilities(GetData()->GrantedAbilities.GetAbilityConfigs());

		for (const FFretteGameplayEffectConfig& EffectConfig : GetData()->GrantedEffects)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(EffectConfig.EffectClass, ASC, 1);
		}
	}
}