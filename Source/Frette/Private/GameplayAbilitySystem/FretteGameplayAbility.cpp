#include "GameplayAbilitySystem/FretteGameplayAbility.h"

UFretteGameplayAbility::UFretteGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFretteGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, GetCurrentSourceObject()->GetName());
}

void UFretteGameplayAbility::OnInputReleased_Implementation()
{

	//Pour logique d'annulation si nécéssaire
}