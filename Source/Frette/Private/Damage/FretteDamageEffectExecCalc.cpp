#include "Damage/FretteDamageEffectExecCalc.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Damage/FretteBodyPartAttributeSet.h"

void UFretteDamageEffectExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag("Frette.GameplayEffect.Damage");
	const float DamageMagnitude = Spec.GetSetByCallerMagnitude(DamageTag, true, 0.0f);
	
	static FGameplayTag BodyPartRootTag = FGameplayTag::RequestGameplayTag(TEXT("Frette.BodyParts"));
	static FGameplayTagContainer BodyPartRoot{ BodyPartRootTag };
	FGameplayTagContainer BodyPartTags = Spec.GetDynamicAssetTags().Filter(BodyPartRoot);
	
	for (const FGameplayTag& Tag : BodyPartTags)
	{
		const UFretteBodyPartAttributeSet* AttributeSet = Frette::BodyParts::GetAttributeSetFromTag(Tag);
		check(IsValid(AttributeSet));
			
		FGameplayModifierEvaluatedData OutputModifier(
			AttributeSet->GetHealthAttribute(),
			EGameplayModOp::Additive,
			DamageMagnitude
		);
		OutExecutionOutput.AddOutputModifier(OutputModifier);
	}
}