#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FretteBodyPartData.generated.h"

class UGameplayEffect;
//Pas l'impression qu'on va avoir assez de regle différente pour justifier cela mais les regles
//Pourrais peut-être être des object a pars entiere qui détermine une regle de trigger
//Quand quelque chose ce produit dans un bodypart on envoi le data du bodypart et le type de dammage 
//a chacune de ces regle pour voir si elle doit être trigger ou pas
//Pourrais permettre des regle spécifique plutot qu'un amas de regles dans un seul struct

USTRUCT(BlueprintType)
struct FBodyPartEffectRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InstantDamageThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CumulativeDamageThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TriggerOnDeath = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasTriggered = false;
};

UCLASS()
class UFretteBodyPartData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag BodyPartTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBodyPartEffectRule> EffectRules;

};