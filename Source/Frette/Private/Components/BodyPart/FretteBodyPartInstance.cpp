#include "Components/BodyPart/FretteBodyPartInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BodyPart/FretteBodyPartContext.h"
#include "Components/BodyPart/EffectRules/FretteDeltaValueRule.h"
#include "Net/UnrealNetwork.h"

void UFretteBodyPartInstance::Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner)
{
	SourceData = InSourceData;
	CurrentHealth = SourceData->MaxHealth;
	OwnerCharacter = Owner;
	BuildRuleLookup();

	SetMinDeltaValueThreshold();
}

//Pour essayer de réduire les lookup inutiles on va chercher le plus petit delta qui peut trigger un éffet et
//Si le changement de la valeur est < on ne vérifiera pas les regle de delta
void UFretteBodyPartInstance::SetMinDeltaValueThreshold()
{
	for (auto DeltaValueRule : GetRulesForEvent(EBodyPartEventType::DeltaValue))
	{
		int CurrentRuleDamageThreshold = Cast<UFretteDeltaValueRule>(DeltaValueRule.Rule)->Threshold;

		if (CurrentRuleDamageThreshold < MinDamageForInstantDamageEffect)
			MinDamageForInstantDamageEffect = CurrentRuleDamageThreshold;
	}
}

void UFretteBodyPartInstance::AddValueByTag(const int Value, const FGameplayTag Tag)
{
	FFretteBodyPartContext Context = FFretteBodyPartContext();

	AccumulatedEffectStackByType.FindOrAdd(Tag) += Value;

	if (Value >= MinDamageForInstantDamageEffect)
	{
		Context.ValueDelta = Value;
		CheckAndApplyRules(EBodyPartEventType::DeltaValue, Tag, Context);
	}

	Context.AccumulatedValue = AccumulatedEffectStackByType[Tag];
	Context.EffectType = Tag;

	CheckAndApplyRules(EBodyPartEventType::AccumulatedValue, Tag, Context);
}

void UFretteBodyPartInstance::CheckAndApplyRules(const EBodyPartEventType EventType, const FGameplayTag Tag, const FFretteBodyPartContext& Context) const
{
	const TArray<FFretteEffectRuleEntry>* Rules = GetRulesForEvent(EventType, Tag);

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
		else if (RuleEntry.Rule->bHasTriggered && !RuleEntry.Rule->CheckCondition(Context))
		{
			RuleEntry.Rule->Reset();
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

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Removed effect %s to body part %s"), *Effect->GetName(), *SourceData->BodyPartTag.ToString()));
	}
}

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

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Applied effect %s to body part %s"), *Effect->GetName(), *SourceData->BodyPartTag.ToString()));

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

		EventTypeToRulesMap.FindOrAdd(RuleInstance->GetRelatedEvent())
		                   .FindOrAdd(RuleInstance->TagType)
		                   .Add(InstancedEntry);
	}
}

const TArray<FFretteEffectRuleEntry>* UFretteBodyPartInstance::GetRulesForEvent(const EBodyPartEventType EventType, const FGameplayTag& EffectTag) const
{
	const TMap<FGameplayTag, TArray<FFretteEffectRuleEntry>>* TagMap = EventTypeToRulesMap.Find(EventType);
	if (!TagMap)
		return nullptr;

	return TagMap->Find(EffectTag);
}

TArray<FFretteEffectRuleEntry> UFretteBodyPartInstance::GetRulesForEvent(const EBodyPartEventType EventType) const
{
	TArray<FFretteEffectRuleEntry> Result;

	const TMap<FGameplayTag, TArray<FFretteEffectRuleEntry>>* TagMap = EventTypeToRulesMap.Find(EventType);
	if (!TagMap)
		return Result;

	for (const auto& [Tag, Rules] : *TagMap)
	{
		Result.Append(Rules);
	}

	return Result;
}

void UFretteBodyPartInstance::OnRep_CurrentHealth() const
{
	UE_LOG(LogTemp, Log, TEXT("MyValue changed to %f"), CurrentHealth);
}