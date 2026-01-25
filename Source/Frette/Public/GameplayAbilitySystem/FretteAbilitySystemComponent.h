

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FretteAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);


/**
 * 
 */
UCLASS()
class FRETTE_API UFretteAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	//Event pour annoncer que le ActorInfo a été setté
	void AbilityActorInfoSet();
	FEffectAssetTags EffectAssetTags;
	
protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
};
