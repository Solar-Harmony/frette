#include "Inventory/Items/Impl/FretteClothingItem.h"

#include "Components/FretteTemperatureComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Inventory/FretteInventoryComponent.h"

void UFretteClothingItem::OnEquipped()
{
	Super::OnEquipped();

	UpdateBodyPartTemperatureModifier(/*bIsEquipping=*/ true);
}

void UFretteClothingItem::OnUnequipped()
{
	Super::OnUnequipped();

	UpdateBodyPartTemperatureModifier(/*bIsEquipping=*/ false);
}

void UFretteClothingItem::UpdateBodyPartTemperatureModifier(const bool bIsEquipping) const
{
	APawn* OwningPawn = Cast<APawn>(GetOwningInventory()->GetOwner());

	const UFretteClotheItemDataAsset* ItemData = Cast<UFretteClotheItemDataAsset>(GetData());

	UFretteBodyPartComponent* BodyPartComponent = Cast<UFretteBodyPartComponent>(OwningPawn->GetComponentByClass(UFretteBodyPartComponent::StaticClass()));

	FGameplayTag AffectedBodyPartTag = ItemData->ItemSlotTag;
	
	const float ModifierToApply = bIsEquipping ? ItemData->ThermalImpedanceModifier : -ItemData->ThermalImpedanceModifier;
	BodyPartComponent->AddValueFromBodyPartTag(AffectedBodyPartTag, ModifierToApply, TAG_BodyPartValues_ThermalImpedance);
}