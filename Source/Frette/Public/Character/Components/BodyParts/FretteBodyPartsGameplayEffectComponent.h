#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "FretteBodyPartsGameplayEffectComponent.generated.h"

UCLASS()
class FRETTE_API UFretteBodyPartsGameplayEffectComponent : public UGameplayEffectComponent
{
    GENERATED_BODY()

public:
	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
        
private:
	// TODO: We need a custom context to provide this or smt
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Frette.BodyPartValues"))
	FGameplayTag Attribute;
};