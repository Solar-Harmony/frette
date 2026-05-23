#include "Character/Components/BodyParts/FretteBodyPartInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/FretteBaseCharacter.h"
#include "Character/Components/BodyParts/FretteBodyPartComponent.h"
#include "Character/Components/BodyParts/FretteBodyPartContext.h"
#include "Character/Components/BodyParts/EffectRules/FretteDeltaValueRule.h"
#include "Frette.h"
#include "Net/UnrealNetwork.h"

void UFretteBodyPartInstance::Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner)
{
	SourceData = InSourceData;

	for (auto Data : SourceData->ValueDatas)
	{
		FindOrAddAccumulatedValue(Data.Type) = Data.StartValue;
	}

	OwnerCharacter = Owner;
	BuildRuleLookup();
	SetMinDeltaValueThreshold();
}

float& UFretteBodyPartInstance::FindOrAddAccumulatedValue(const FGameplayTag& Tag)
{
	FFretteAccumulatedValueEntry* Existing = AccumulatedValues.FindByPredicate(
		[&Tag](const FFretteAccumulatedValueEntry& Entry) { return Entry.Tag == Tag; });

	if (Existing)
		return Existing->Value;

	FFretteAccumulatedValueEntry& NewEntry = AccumulatedValues.AddDefaulted_GetRef();
	NewEntry.Tag = Tag;

	return NewEntry.Value;
}

//Pour essayer de réduire les lookup inutiles on va chercher le plus petit delta qui peut trigger un éffet et
//Si le changement de la valeur est < on ne vérifiera pas les regle de delta
void UFretteBodyPartInstance::SetMinDeltaValueThreshold()
{
	for (auto DeltaValueRule : GetRulesForEvent(EBodyPartEventType::DeltaValue))
	{
		const UFretteDeltaValueRule* Rule = Cast<UFretteDeltaValueRule>(DeltaValueRule.Rule);

		ensureMsgf(Rule, TEXT("Rule for DeltaValue event is not a UFretteDeltaValueRule"));

		if (Rule->Threshold < MinValueDelta)
			MinValueDelta = Rule->Threshold;
	}
}

FFretteBodyPartContext UFretteBodyPartInstance::AddValueByTag(const float Value, const FGameplayTag Tag)
{
	const UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	if (!ensureMsgf(OwnerASC, TEXT("Owner character does not have an ability system component.")))
		return FFretteBodyPartContext();

	const float ClampedDelta = ClampDelta(Value, Tag);

	FFretteBodyPartContext Context;
	Context.AccumulatedValue = FindOrAddAccumulatedValue(Tag);
	Context.ValueDelta = ClampedDelta;
	
	if (ClampedDelta == 0.f)
		return Context;

	const float CurrentValue = FindOrAddAccumulatedValue(Tag) += ClampedDelta;
	
	FRETTE_LOG(Log, "%s of %s's %s changed by %f, now %f", *Tag.ToString(), *OwnerCharacter->GetName(), *GetBodyPartTag().ToString(), ClampedDelta, CurrentValue);
	
	CheckDeltaRules(Tag, Context, ClampedDelta);
	CheckAccumulatedValueRules(Tag, Context, CurrentValue);
	
	return Context;
}

float UFretteBodyPartInstance::ClampDelta(const float Value, const FGameplayTag Tag)
{
	const float AccumulatedValue = FindOrAddAccumulatedValue(Tag);
	const float NewValue = AccumulatedValue + Value;

	const float Min = SourceData->GetMinValueForType(Tag);
	const float Max = SourceData->GetMaxValueForType(Tag);
	float ClampedDelta;

	if (NewValue < Min || NewValue > Max)
	{
		ClampedDelta = FMath::Clamp(NewValue, Min, Max) - AccumulatedValue;
	}
	else
	{
		ClampedDelta = Value;
	}

	return ClampedDelta;
}

void UFretteBodyPartInstance::CheckDeltaRules(const FGameplayTag Tag, FFretteBodyPartContext& Context, const float ClampedDelta) const
{
	float PositiveClampedValue = FMath::Abs(ClampedDelta);

	if (PositiveClampedValue >= MinValueDelta)
	{
		Context.ValueDelta = PositiveClampedValue;
		CheckAndApplyRules(EBodyPartEventType::DeltaValue, Tag, Context);
	}
}

void UFretteBodyPartInstance::CheckAccumulatedValueRules(const FGameplayTag Tag, FFretteBodyPartContext& Context, const float CurrentValue) const
{
	Context.AccumulatedValue = CurrentValue;
	Context.EffectType = Tag;

	CheckAndApplyRules(EBodyPartEventType::AccumulatedValue, Tag, Context);

	Context.SourceData = SourceData;
	CheckAndApplyRules(EBodyPartEventType::LimitReached, Tag, Context);
}
//BUG: Si deux partie donne le même éffet de mort un va donner l'éffet et l'autre va remove l'éffet car lui n'est pas a un max/min
void UFretteBodyPartInstance::CheckAndApplyRules(const EBodyPartEventType EventType, const FGameplayTag Tag, const FFretteBodyPartContext& Context) const
{
	const TArray<FFretteEffectRuleEntry>* Rules = GetRulesForEvent(EventType, Tag);

	if (!Rules)
		return;

	for (const FFretteEffectRuleEntry& RuleEntry : *Rules)
	{
		if (!RuleEntry.Rule || (RuleEntry.Effects.Num() == 0 && RuleEntry.Abilities.Num() == 0))
			continue;

		if (RuleEntry.Rule->CheckIfTriggers(Context))
		{
			ApplyGameplayEffects(RuleEntry.Effects);
			ApplyGameplayAbilities(RuleEntry.Abilities);
		}
		else if (RuleEntry.Rule->bHasTriggered && !RuleEntry.Rule->CheckCondition(Context))
		{
			RuleEntry.Rule->Reset();
			RemoveGameplayEffects(RuleEntry.Effects);
			RemoveGameplayAbilities(RuleEntry.Abilities);
		}
	}
}

void UFretteBodyPartInstance::ApplyGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>> Effects) const
{
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	if (!ensureMsgf(OwnerASC, TEXT("Owner character does not have an ability system component.")))
		return;

	for (const auto& Effect : Effects)
	{
		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		const FGameplayEffectSpecHandle NewHandle = OwnerASC->MakeOutgoingSpec(Effect, 1, EffectContext);

		OwnerASC->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());

		FRETTE_LOG(Log, "%s's %s received effect '%s'", *OwnerCharacter->GetName(), *GetBodyPartTag().ToString(), *Effect->GetName());
	}
}

void UFretteBodyPartInstance::RemoveGameplayEffects(TArray<TSubclassOf<UGameplayEffect>> Effects) const
{
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	
	if (!ensureMsgf(OwnerASC, TEXT("Owner character does not have an ability system component.")))
		return;

	for (const TSubclassOf<UGameplayEffect>& Effect : Effects)
	{
		OwnerASC->RemoveActiveGameplayEffectBySourceEffect(Effect, OwnerASC, 1);

		FRETTE_LOG(Log, "%s's %s lost effect '%s'", *OwnerCharacter->GetName(), *GetBodyPartTag().ToString(), *Effect->GetName());
	}
}

void UFretteBodyPartInstance::ApplyGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities) const
{
	UAbilitySystemComponent* OwnerASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	if (!ensureMsgf(OwnerASC, TEXT("Owner character does not have an ability system component.")))
		return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		if (!AbilityClass)
			continue;

		FGameplayEventData EventData;
		EventData.Instigator = OwnerCharacter;

		FGameplayAbilitySpec Spec(AbilityClass, 1);
		OwnerASC->GiveAbilityAndActivateOnce(Spec, &EventData);

		FRETTE_LOG(Log, "%s gained gameplay ability '%s'.", *OwnerCharacter->GetName(), *AbilityClass->GetName());
	}
}

//On va chercher l'habileté par classe pour ne pas avoir a garder un liste des handles des habiletés appliqué
//Fait basically la même chose que le RemoveActiveGameplayEffectBySourceEffect
void UFretteBodyPartInstance::RemoveGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities) const
{
	UAbilitySystemComponent* OwnerASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	if (!ensureMsgf(OwnerASC, TEXT("Owner character does not have an ability system component.")))
		return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		FGameplayAbilitySpec* Spec = OwnerASC->FindAbilitySpecFromClass(AbilityClass);

		if (Spec)
		{
			OwnerASC->ClearAbility(Spec->Handle);

			FRETTE_LOG(Log, "%s lost gameplay ability '%s'.", *OwnerCharacter->GetName(), *AbilityClass->GetName());
		}
	}
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
		InstancedEntry.Abilities = RuleEntry.Abilities;

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

void UFretteBodyPartInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFretteBodyPartInstance, AccumulatedValues);
	DOREPLIFETIME(UFretteBodyPartInstance, SourceData);
	DOREPLIFETIME(UFretteBodyPartInstance, OwnerCharacter);
}

void UFretteBodyPartInstance::OnRep_AccumulatedValues()
{
	if (UFretteBodyPartComponent* Comp = Cast<UFretteBodyPartComponent>(GetOuter()))
	{
		Comp->OnBodyPartsInitialized.Broadcast();
	}
}
