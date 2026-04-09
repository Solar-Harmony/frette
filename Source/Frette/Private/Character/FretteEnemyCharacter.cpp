#include "Character/FretteEnemyCharacter.h"

#include "Perception/AIPerceptionComponent.h"

AFretteEnemyCharacter::AFretteEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UFretteAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AiPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AiPerceptionComponent");
}

void AFretteEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
}

void AFretteEnemyCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}