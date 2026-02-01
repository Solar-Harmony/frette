#pragma once

#include "CoreMinimal.h"
#include "AbilitySetDataAsset.h"
#include "AbilitySystemComponent.h"
#include "FretteAbilitySystemComponent.generated.h"

UCLASS()
class FRETTE_API UFretteAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
	void GrantAbilitiesFromLoadout(UAbilitySetDataAsset* Loadout);

protected:
	//Pourrais peut-être avoir le input action directement plutot que le gameplayTag 
	//Mais le system de tag permet aussi de l'utiliser pour les IA facilement si jamais on veux
	UPROPERTY()
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> InputAbilityMap;
};