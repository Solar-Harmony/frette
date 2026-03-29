#include "Inventory/Items/Impl/FretteClothingItem.h"

#include "Components/FretteTemperatureComponent.h"
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

	UFretteTemperatureComponent* TemperatureComponent = Cast<UFretteTemperatureComponent>(OwningPawn->GetComponentByClass(UFretteTemperatureComponent::StaticClass()));

	FGameplayTag AffectedBodyPartTag = ItemData->ItemSlotTag;
	const int ModifierToApply = bIsEquipping ? ItemData->TemperatureModifier : -ItemData->TemperatureModifier;
	TemperatureComponent->AddBodyPartTemperatureModifier(ModifierToApply, AffectedBodyPartTag);
}