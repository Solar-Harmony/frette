


#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"

#include "GameplayAbilitySystem/FretteGameplayAbility.h"

void UFretteAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

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
	if (!InputTag.IsValid()) return;

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
				UE_LOG(LogAbilitySystemComponent,Error,TEXT("Ability %s input released called but ability is not InstancedPerActor, OnInputReleased will not be called"),*AbilitySpec->Ability->GetName());
			}
		}
	}
}

void UFretteAbilitySystemComponent::GrantAbilitiesFromLoadout(UDA_ClassLoadout* Loadout)
{
	if (!Loadout) return;
    
	for (const FAbilityTagMapping& Mapping : Loadout->AbilityMappings)
	{
		FGameplayAbilitySpec Spec(Mapping.Ability, Mapping.AbilityLevel);
        
		Spec.GetDynamicSpecSourceTags().AddTag(Mapping.InputTag);
        
		FGameplayAbilitySpecHandle Handle = GiveAbility(Spec);
        
		InputAbilityMap.Add(Mapping.InputTag, Handle);
	}
}
