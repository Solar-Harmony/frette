

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DA_ClassLoadout.h"
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
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
	void GrantAbilitiesFromLoadout(UDA_ClassLoadout* Loadout);
	
protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const;
	
	UPROPERTY()
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> InputAbilityMap;
};
