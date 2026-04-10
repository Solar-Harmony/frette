#include "Character/FretteEnemyCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/PawnSensingComponent.h"

AFretteEnemyCharacter::AFretteEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UFretteAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AiPawnSensing = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
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

void AFretteEnemyCharacter::Die()
{
	Super::Die();

	if (!HasAuthority())
		return;

	Multicast_HandleDeath(GetCharacterMovement()->Velocity);
}

void AFretteEnemyCharacter::Multicast_HandleDeath_Implementation(FVector FinalVelocity)
{
	Super::Multicast_HandleDeath_Implementation(FinalVelocity);

	GetMesh()->Stop();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));

	//TODO: Faire un parent bone string field qui permet de déterminer le bone à partir duquel le ragdoll doit être appliqué
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("hips"), true, true);
	GetMesh()->SetPhysicsLinearVelocity(FinalVelocity, false, FName("hips"));

	AiPawnSensing->Deactivate();
	//retirer le mouvement de l'ennemis
	//Arreter le pawn sensing
}