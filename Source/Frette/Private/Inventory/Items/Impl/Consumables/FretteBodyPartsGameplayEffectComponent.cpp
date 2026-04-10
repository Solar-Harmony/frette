#include "Inventory/Items/Impl/Consumables/FretteBodyPartsGameplayEffectComponent.h"

#include "Character/FrettePlayerCharacter.h"

bool UFretteBodyPartsGameplayEffectComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	AActor* Owner = ActiveGEContainer.Owner->GetOwnerActor();
	return Cast<AFrettePlayerCharacter>(Owner) != nullptr;
}

void UFretteBodyPartsGameplayEffectComponent::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	const AFrettePlayerCharacter* Owner = CastChecked<AFrettePlayerCharacter>(ActiveGEContainer.Owner->GetOwnerActor());
	UFretteBodyPartComponent* BodyParts = Owner->BodyPartComponent;
	
	for (const FFretteBodyPartGameplayEffectConfig& Config : BodyPartEffectConfigs)
	{
		// TODO: We address body part values with 2 tags but SetByCaller uses 1 tag only
		// so we can only apply either health or temperature on one body part per effect
		// maybe use a custom context, or accept the limitation
		const float Value = GESpec.GetSetByCallerMagnitude(Config.BodyPart, true, 0.0f);
		const int32 IntValue = FMath::TruncToInt32(Value);
		
		BodyParts->AddValueFromBodyPartTag(Config.BodyPart, IntValue, Config.Attribute);
	}
}