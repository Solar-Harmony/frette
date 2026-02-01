#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilitySetDataAsset.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAbilityTagMapping
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;
};

//Je suis pas 100% sur de comment nos classes vont fonctionner mais ça feelais quand même relativement simple de le faire demême for now
//Genre peut-être qu'on va pas avoir de hotbar avec des abilities assigné aux touches mais juste des objets qu'on équipe donc activer
//Une abilité va toujours être clic gauche avec l'objet équipé

//Live ça permet de juste équiper un loadout de base et ça détermine ce que chaque touche va faire selon l'abilité associé
//Donc on peut facilement juste changer de loadout d'abileté selon la classe mais les changement at runtime son surement plus chiant
//Faudra voir ça
UCLASS()
class FRETTE_API UAbilitySetDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAbilityTagMapping> AbilityMappings;

	//Si on veut mettre des passifs différent selon la classe
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
};