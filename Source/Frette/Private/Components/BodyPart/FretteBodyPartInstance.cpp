#include "Components/BodyPart/FretteBodyPartInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BodyPart/FretteBodyPartContext.h"
#include "Components/BodyPart/EffectRules/FretteSingleDamageInstanceRule.h"
#include "Net/UnrealNetwork.h"

void UFretteBodyPartInstance::Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner)
{
	SourceData = InSourceData;
	CurrentHealth = SourceData->MaxHealth;
	OwnerCharacter = Owner;
	BuildRuleLookup();

	SetMinDamageThresholdForInstantEffect();
}

//Pour essayer de réduire les lookup inutiles
void UFretteBodyPartInstance::SetMinDamageThresholdForInstantEffect()
{
	for (auto InstantDamageRule : *GetRulesForEvent(EBodyPartEventType::InstantDamage))
	{
		int CurrentRuleDamageThreshold = Cast<UFretteSingleDamageInstanceRule>(InstantDamageRule.Rule)->DamageThreshold;

		if (CurrentRuleDamageThreshold < MinDamageForInstantDamageEffect)
			MinDamageForInstantDamageEffect = CurrentRuleDamageThreshold;
	}
}

void UFretteBodyPartInstance::ApplyDamage(const float Damage)
{
	CurrentHealth -= Damage;

	FFretteBodyPartContext Context = FFretteBodyPartContext();

	if (Damage >= MinDamageForInstantDamageEffect)
	{
		Context.InstantDamage = Damage;
		CheckAndApplyRules(EBodyPartEventType::InstantDamage, Context);
	}

	Context.RemainingHealth = CurrentHealth;

	CheckAndApplyRules(EBodyPartEventType::RemainingHealth, Context);
}

//Ajout de stack temperature (le cold retire des stacks et la chaleur augmente les stacks)
void UFretteBodyPartInstance::AddValueByTag(const int Value, const FGameplayTag Tag)
{
	FFretteBodyPartContext Context = FFretteBodyPartContext();

	AccumulatedEffectStackByType.FindOrAdd(Tag) += Value;

	Context.CumulativeValue = AccumulatedEffectStackByType[Tag];
	Context.EffectType = Tag;

	//TODO:Devrais faire le check des regle selon le EffectType aussi donc on va juste regarder les regle qui sont affecter par la temperature
	//TODO:Devrais retirer les gameplay effects pour les éffets qui ne sont plus activer car les stacks on changer
	CheckAndApplyRules(EBodyPartEventType::StatusEffect, Context);
}

void UFretteBodyPartInstance::CheckAndApplyRules(const EBodyPartEventType EventType, const FFretteBodyPartContext& Context) const
{
	const TArray<FFretteEffectRuleEntry>* Rules = GetRulesForEvent(EventType);

	if (!Rules)
		return;

	for (const FFretteEffectRuleEntry& RuleEntry : *Rules)
	{
		if (!RuleEntry.Rule || RuleEntry.Effects.Num() == 0)
			continue;

		if (RuleEntry.Rule->CheckIfTriggers(Context))
		{
			ApplyGameplayEffects(RuleEntry.Effects);
		}
		else
		{
			//Retire les éffet qui on été appliquer et qui ne respectent plus la regle qui l'a appliquer
			if (RuleEntry.Rule->bHasTriggered)
				RemoveGameplayEffects(RuleEntry.Effects);
		}

	}
}

void UFretteBodyPartInstance::RemoveGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> Effects) const
{
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	for (TSubclassOf Effect : Effects)
	{
		OwnerASC->RemoveActiveGameplayEffectBySourceEffect(Effect, OwnerASC, 1);
	}
}

//TODO: Ajouter le soin de parties du corps, retirer les éffets qui on été ajouté et reset les infos (hasTriggered, accumulatedDamageByType)

void UFretteBodyPartInstance::ApplyGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>> Effects) const
{
	for (const auto& Effect : Effects)
	{
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		//On pourrais utiliser le niveau ici pour l'intensité de l'éffet selon le nombre de stack
		const FGameplayEffectSpecHandle NewHandle = OwnerASC->MakeOutgoingSpec(Effect, 1, EffectContext);

		OwnerASC->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Applied effect %s to body part %s"), *Effect->GetName(), *SourceData->BodyPartTag.ToString()));

	}
}

void UFretteBodyPartInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFretteBodyPartInstance, CurrentHealth);
}

void UFretteBodyPartInstance::BuildRuleLookup()
{
	EventTypeToRulesMap.Empty();
	AllRuleInstances.Empty();

	for (const FFretteEffectRuleEntry& RuleEntry : SourceData->EffectRules)
	{
		if (!RuleEntry.Rule)
			continue;

		UFretteBodyPartEffectRule* RuleInstance = DuplicateObject<UFretteBodyPartEffectRule>(RuleEntry.Rule, this);
		if (!RuleInstance)
			continue;

		AllRuleInstances.Add(RuleInstance);

		FFretteEffectRuleEntry InstancedEntry;
		InstancedEntry.Rule = RuleInstance;
		InstancedEntry.Effects = RuleEntry.Effects;

		EventTypeToRulesMap.FindOrAdd(RuleInstance->GetRelatedEvent()).Add(InstancedEntry);
	}
}

const TArray<FFretteEffectRuleEntry>* UFretteBodyPartInstance::GetRulesForEvent(const EBodyPartEventType EventType) const
{
	return EventTypeToRulesMap.Find(EventType);
}

void UFretteBodyPartInstance::OnRep_CurrentHealth() const
{
	UE_LOG(LogTemp, Log, TEXT("MyValue changed to %f"), CurrentHealth);
}