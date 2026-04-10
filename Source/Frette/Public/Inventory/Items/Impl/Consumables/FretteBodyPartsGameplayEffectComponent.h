#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "GameplayTagContainer.h"
#include "FretteBodyPartsGameplayEffectComponent.generated.h"

USTRUCT()
struct FFretteBodyPartGameplayEffectConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, meta=(Categories="Frette.BodyPart"))
    FGameplayTag BodyPart;

    UPROPERTY(EditDefaultsOnly, meta=(Categories="Frette.BodyPartValue"))
    FGameplayTag Attribute;
};

UCLASS()
class FRETTE_API UFretteBodyPartsGameplayEffectComponent : public UGameplayEffectComponent
{
    GENERATED_BODY()

public:
	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
        
private:
    UPROPERTY(EditDefaultsOnly)
    TArray<FFretteBodyPartGameplayEffectConfig> BodyPartEffectConfigs;
};