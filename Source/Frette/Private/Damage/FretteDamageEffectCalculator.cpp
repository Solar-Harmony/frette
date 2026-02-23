#include "Damage/FretteDamageEffectCalculator.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Damage/FretteBodyPartAttributeSet.h"
#include "Damage/FretteBodyPartTags.h"

void UFretteDamageEffectCalculator::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag("Frette.GameplayEffect.Damage");
	const float DamageMagnitude = Spec.GetSetByCallerMagnitude(DamageTag, true, 0.0f);
	
	static FGameplayTag BodyPartRootTag = FGameplayTag::RequestGameplayTag(TEXT("Frette.BodyParts"));
	static FGameplayTagContainer BodyPartRoot{ BodyPartRootTag };
	FGameplayTagContainer BodyPartTags = Spec.GetDynamicAssetTags().Filter(BodyPartRoot);
	
	for (const FGameplayTag& Tag : BodyPartTags)
	{
		const UFretteBodyPartAttributeSet* AttributeSet = Frette::BodyParts::GetAttributeSetFromTag(TargetASC, Tag);
		check(IsValid(AttributeSet));
			
		OutExecutionOutput.AddOutputModifier({
			AttributeSet->GetHealthAttribute(), 
			EGameplayModOp::Additive, 
			DamageMagnitude
		});
	}
}