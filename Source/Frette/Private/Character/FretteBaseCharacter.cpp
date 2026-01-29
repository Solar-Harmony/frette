


#include "Character/FretteBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

AFretteBaseCharacter::AFretteBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFretteBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* AFretteBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AFretteBaseCharacter::ApplyDefaultAttributeEffect(const FGameplayEffectContextHandle& EffectContext) const
{
	const FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, CharacterLevel, EffectContext);
	
	if (NewHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AFretteBaseCharacter::ApplyDefaultStartupEffect(const FGameplayEffectContextHandle& EffectContext) const
{
	for (TSubclassOf Effect : StartupEffects)	
	{
		const FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, CharacterLevel, EffectContext);
		if (NewHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}
}

void AFretteBaseCharacter::InitAbilityActorInfo()
{
	check(AbilitySystemComponent)

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	ApplyDefaultAttributeEffect(EffectContext);
	ApplyDefaultStartupEffect(EffectContext);
}

