#include "Inventory/Items/Impl/Consumables/FretteBodyPartsGameplayEffectComponent.h"

#include "Character/FrettePlayerCharacter.h"

bool UFretteBodyPartsGameplayEffectComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	AActor* Owner = ActiveGEContainer.Owner->GetAvatarActor();
	return Cast<AFrettePlayerCharacter>(Owner) != nullptr;
}

void UFretteBodyPartsGameplayEffectComponent::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	const AFrettePlayerCharacter* Owner = CastChecked<AFrettePlayerCharacter>(ActiveGEContainer.Owner->GetAvatarActor());
	UFretteBodyPartComponent* BodyParts = Owner->BodyPartComponent;
	
	for (auto Pair : GESpec.SetByCallerTagMagnitudes)
	{
		// TODO: check is Frette.BodyPart
		const float Value = Pair.Value;
		BodyParts->AddValueFromBodyPartTag(Pair.Key, Value, Attribute);
	}
}