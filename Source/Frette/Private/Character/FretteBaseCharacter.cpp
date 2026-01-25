


#include "Character/FretteBaseCharacter.h"

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

void AFretteBaseCharacter::InitAbilityActorInfo()
{
}

