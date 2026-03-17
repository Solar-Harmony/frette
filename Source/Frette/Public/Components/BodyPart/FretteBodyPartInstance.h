#pragma once

#include "CoreMinimal.h"
#include "FretteBodyPartData.h"
#include "FretteBodyPartEffectRule.h"
#include "Character/FretteBaseCharacter.h"
#include "UObject/Object.h"
#include "FretteBodyPartInstance.generated.h"

struct FGameplayTag;
class UFretteBodyPartData;

//Pas un fan des context objects mais c'Est le plus simple que j'ai trouver pour send l'info aux regles
//Sans juste donner acces a l'instance du body part
//Pourrais peut-être juste cast la regle au type précis de la regle vu que chaque regle est associé a un event spécifique

UCLASS()
class FRETTE_API UFretteBodyPartInstance : public UObject
{
	GENERATED_BODY()

public:
	FGameplayTag GetAssociatedTag() const { return SourceData->BodyPartTag; }
	void Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner);
	void AddValueByTag(int Value, FGameplayTag Tag);
	void CheckAndApplyRules(EBodyPartEventType EventType, FGameplayTag Tag, const FFretteBodyPartContext& Context) const;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth, BlueprintReadOnly)
	float CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth() const;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, int> AccumulatedValuesByType;

private:
	void ApplyGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> Effects) const;
	void RemoveGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> Effects) const;
	void BuildRuleLookup();
	void SetMinDeltaValueThreshold();
	const TArray<FFretteEffectRuleEntry>* GetRulesForEvent(const EBodyPartEventType EventType, const FGameplayTag& EffectTag) const;
	TArray<FFretteEffectRuleEntry> GetRulesForEvent(const EBodyPartEventType EventType) const;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartData> SourceData;

	UPROPERTY()
	TObjectPtr<AFretteBaseCharacter> OwnerCharacter;

	int MinDamageForInstantDamageEffect = 9999;

	// Nécéssaire pour garder les regles instancier en vie sinon ils se font garbage collect
	// et ça fait des crash quand on essaye d'y accéder
	UPROPERTY()
	TArray<TObjectPtr<UFretteBodyPartEffectRule>> AllRuleInstances;

	//Je suis aller avec le TMap de event types car j'avais peur d'avoir des éffet qui applique des dégat
	//ou des stack de statusEffect de manière fréquente (Cold stacks) et je voullais pas avoir a passer a travers
	//tous les regles de déclanchement a chaque fois
	TMap<EBodyPartEventType, TMap<FGameplayTag, TArray<FFretteEffectRuleEntry>>> EventTypeToRulesMap;

};