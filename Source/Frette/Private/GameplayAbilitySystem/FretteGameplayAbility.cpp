


#include "GameplayAbilitySystem/FretteGameplayAbility.h"

UFretteGameplayAbility::UFretteGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
}

void UFretteGameplayAbility::OnInputReleased_Implementation()
{
	//Pour logique d'annulation si nécéssaire
}


