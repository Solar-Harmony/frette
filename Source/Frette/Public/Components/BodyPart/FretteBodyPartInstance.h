#pragma once

#include "CoreMinimal.h"
#include "FretteBodyPartData.h"
#include "FretteBodyPartEffectRule.h"
#include "Character/FretteBaseCharacter.h"
#include "UObject/Object.h"
#include "FretteBodyPartInstance.generated.h"

struct FGameplayTag;
class UFretteBodyPartData;

USTRUCT()
struct FFretteAccumulatedValueEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 Value = 0;
};

UCLASS()
class FRETTE_API UFretteBodyPartInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsSupportedForNetworking() const override { return true; }

	FGameplayTag GetBodyPartTag() const { return SourceData->BodyPartTag; }

	void Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner);
	int32& FindOrAddAccumulatedValue(const FGameplayTag& Tag);
	FFretteBodyPartContext AddValueByTag(int Value, FGameplayTag Tag);
	void CheckAndApplyRules(EBodyPartEventType EventType, FGameplayTag Tag, const FFretteBodyPartContext& Context) const;

	UPROPERTY(ReplicatedUsing=OnRep_AccumulatedValues)
	TArray<FFretteAccumulatedValueEntry> AccumulatedValues;
	
	UFretteBodyPartData* GetSourceData() const { return SourceData; }

	UFUNCTION()
	void OnRep_AccumulatedValues();

private:
	void ApplyGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> Effects) const;
	void RemoveGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> Effects) const;
	void ApplyGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities) const;
	void RemoveGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities) const;
	void BuildRuleLookup();
	void SetMinDeltaValueThreshold();
	int ClampDelta(int Value, FGameplayTag Tag);
	void CheckDeltaRules(FGameplayTag Tag, FFretteBodyPartContext& Context, int ClampedDelta) const;
	void CheckAccumulatedValueRules(FGameplayTag Tag, FFretteBodyPartContext& Context, int CurrentValue) const;
	const TArray<FFretteEffectRuleEntry>* GetRulesForEvent(const EBodyPartEventType EventType, const FGameplayTag& EffectTag) const;
	TArray<FFretteEffectRuleEntry> GetRulesForEvent(const EBodyPartEventType EventType) const;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TObjectPtr<UFretteBodyPartData> SourceData;

	UPROPERTY(Replicated)
	TObjectPtr<AFretteBaseCharacter> OwnerCharacter;

	int MinValueDelta = 9999;

	// Nécéssaire pour garder les regles instancier en vie sinon ils se font garbage collect
	// et ça fait des crash quand on essaye d'y accéder
	UPROPERTY()
	TArray<TObjectPtr<UFretteBodyPartEffectRule>> AllRuleInstances;

	//Je suis aller avec le TMap de event types car j'avais peur d'avoir des éffet qui applique des dégat
	//ou des stack de statusEffect de manière fréquente (Cold stacks) et je voullais pas avoir a passer a travers
	//tous les regles de déclanchement a chaque fois
	TMap<EBodyPartEventType, TMap<FGameplayTag, TArray<FFretteEffectRuleEntry>>> EventTypeToRulesMap;

	TMap<TSubclassOf<UGameplayAbility>, FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};