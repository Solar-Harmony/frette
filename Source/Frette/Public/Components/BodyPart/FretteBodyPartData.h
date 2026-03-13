#pragma once

#include "CoreMinimal.h"
#include "FretteBodyPartEffectRule.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FretteBodyPartData.generated.h"

class UGameplayEffect;

//Je sais pas trop si je devrais juste avoir les effect a déclancher directement dans la regle
USTRUCT(BlueprintType)
struct FFretteEffectRuleEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UFretteBodyPartEffectRule> Rule = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> Effects;
};

UCLASS()
class UFretteBodyPartData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Meta = (Categories = "Frette"))
	FGameplayTag BodyPartTag;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere)
	TArray<FFretteEffectRuleEntry> EffectRules;

};