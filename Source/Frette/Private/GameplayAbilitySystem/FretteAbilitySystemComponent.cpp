


#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"

void UFretteAbilitySystemComponent::AbilityActorInfoSet()
{
	//Trigger quand un effet est appliquer sur le owner de ce AbilitySystemComponent
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UFretteAbilitySystemComponent::EffectApplied);
}

void UFretteAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
