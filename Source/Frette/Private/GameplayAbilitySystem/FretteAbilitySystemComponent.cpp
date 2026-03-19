#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/FretteBaseCharacter.h"
#include "Frette/Frette.h"
#include "GameplayAbilitySystem/FretteGameplayAbility.h"

void UFretteAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid() || !InputAbilityMap.Find(InputTag))
		return;

	if (const FGameplayAbilitySpecHandle* Handle = InputAbilityMap.Find(InputTag); Handle->IsValid())
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(*Handle);

		AbilitySpecInputPressed(*AbilitySpec);

		if (!AbilitySpec->IsActive())
		{
			TryActivateAbility(AbilitySpec->Handle);
		}
	}
}

void UFretteAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid() || !InputAbilityMap.Find(InputTag))
		return;

	if (const FGameplayAbilitySpecHandle* Handle = InputAbilityMap.Find(InputTag); Handle->IsValid())
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(*Handle);

		AbilitySpecInputReleased(*AbilitySpec);
		if (AbilitySpec->IsActive())
		{
			//Assume que l'Abilité va être instanced per actor et non pas per execution
			//(Si il est par exécution on va avoir un array d'instance qu'il va falloir gerer)
			if (AbilitySpec->Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
			{
				Cast<UFretteGameplayAbility>(AbilitySpec->GetPrimaryInstance())->OnInputReleased();
			}
			else
			{
				//Pourrais peut-être être gerer si on passe a travers 
				//tout les instances de l'abilité et call le OnInputReleased sur chacune
				UE_LOG(LogAbilitySystemComponent, Error, TEXT("Ability %s input released called but ability is not InstancedPerActor, OnInputReleased will not be called"), *AbilitySpec->Ability->GetName());
			}
		}
	}
}

void UFretteAbilitySystemComponent::GrantAbility(const FFretteGameplayAbilityConfig& AbilityConfig, UObject* SourceObject)
{
	require(AbilityConfig.IsValid(), "Tried to apply an invalid ability config.");
	
	FGameplayAbilitySpec Spec(AbilityConfig.Ability, AbilityConfig.AbilityLevel);
	Spec.SourceObject = SourceObject;
	Spec.GetDynamicSpecSourceTags().AddTag(AbilityConfig.InputTag);

	GiveAbility(Spec);
}

void UFretteAbilitySystemComponent::RevokeAbility(const FFretteGameplayAbilityConfig& AbilityConfig)
{
	require(AbilityConfig.IsValid(), "Tried to revoke an invalid ability config.");
	
	const FGameplayAbilitySpecHandle* Handle = InputAbilityMap.Find(AbilityConfig.InputTag);
	require(Handle != nullptr && Handle->IsValid(), "Tried to revoke an ability that wasn't granted.");

	ClearAbility(*Handle);
}

void UFretteAbilitySystemComponent::GrantAbilities(const TArray<FFretteGameplayAbilityConfig>& AbilityConfigs, UObject* SourceObject)
{
	for (const FFretteGameplayAbilityConfig& Config : AbilityConfigs)
	{
		GrantAbility(Config, SourceObject);
	}
}

void UFretteAbilitySystemComponent::RevokeAbilities(const TArray<FFretteGameplayAbilityConfig>& AbilityConfigs)
{
	for (const FFretteGameplayAbilityConfig& Config : AbilityConfigs)
	{
		RevokeAbility(Config);
	}
}

void UFretteAbilitySystemComponent::GrantAbilities(const UFretteAbilitySetDataAsset* AbilitySet, UObject* SourceObject)
{
	require(AbilitySet, "Tried to grant abilities from a null ability set.");
	GrantAbilities(AbilitySet->AbilityConfigs, SourceObject);
}

void UFretteAbilitySystemComponent::RevokeAbilities(const UFretteAbilitySetDataAsset* AbilitySet)
{
	require(AbilitySet, "Tried to revoke abilities from a null ability set.");
	RevokeAbilities(AbilitySet->AbilityConfigs);
}

void UFretteAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	const FGameplayTagContainer& SourceTags = AbilitySpec.GetDynamicSpecSourceTags();

	for (const FGameplayTag& Tag : SourceTags)
	{
		InputAbilityMap.Add(Tag, AbilitySpec.Handle);
	}
}

void UFretteAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	
	const FGameplayTagContainer& SourceTags = AbilitySpec.GetDynamicSpecSourceTags();
	
	for (const FGameplayTag& Tag : SourceTags)
	{
		InputAbilityMap.Remove(Tag);
	}
}


FActiveGameplayEffectHandle UFretteAbilitySystemComponent::ApplyEffect(const FFretteGameplayEffectConfig& Config, const UObject* SourceObject)
{
	FGameplayEffectContextHandle Context = MakeEffectContext();
	Context.AddSourceObject(SourceObject);
	
	const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(Config.EffectClass, Config.Level, Context);
	
	for (const TPair<FGameplayTag, float>& Magnitude : Config.Magnitudes)
	{
		Spec.Data.Get()->SetSetByCallerMagnitude(Magnitude.Key, Magnitude.Value);
	}
	
	return ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

UFretteAbilitySystemComponent* UFretteAbilitySystemComponent::Get(class AFretteBaseCharacter* Character)
{
	return Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character));
}