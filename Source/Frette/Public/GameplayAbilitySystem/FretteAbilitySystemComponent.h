#pragma once

#include "CoreMinimal.h"
#include "AbilitySetDataAsset.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FretteAbilitySystemComponent.generated.h"

USTRUCT()
struct FFretteGameplayEffectConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> EffectClass;
	
	UPROPERTY(EditAnywhere)
	float Level = 1.0f;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> Magnitudes;
};

UCLASS()
class FRETTE_API UFretteAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
	void GrantAbilitiesFromAbilitySet(UAbilitySetDataAsset* Loadout, UObject* SourceObject);
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	void RemoveAbilitiesFromAbilitySet(UAbilitySetDataAsset* Loadout);
	
	FActiveGameplayEffectHandle ApplyEffect(const FFretteGameplayEffectConfig& Config, const UObject* SourceObject);
	
	static UFretteAbilitySystemComponent* Get(class AFretteBaseCharacter* Character) { return Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character)); }

protected:
	//Pourrais peut-être avoir le input action directement plutot que le gameplayTag 
	//Mais le system de tag permet aussi de l'utiliser pour les IA facilement si jamais on veux
	UPROPERTY()
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> InputAbilityMap;
};