#include "Character/FretteEnemyCharacter.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISenseConfig_Sight.h"

class UNavigationSystemV1;

AFretteEnemyCharacter::AFretteEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AbilitySystemComponent = CreateDefaultSubobject<UFretteAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AiPerception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2500.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AiPerception->ConfigureSense(*SightConfig);
	AiPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AFretteEnemyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFretteEnemyCharacter, WaitingAtPatrolPoint);
}

void AFretteEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	AiPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AFretteEnemyCharacter::OnTargetPerceived);
}

void AFretteEnemyCharacter::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	AFrettePlayerCharacter* Player = Cast<AFrettePlayerCharacter>(Actor);

	if (!Player)
		return;

	if (Stimulus.WasSuccessfullySensed())
	{
		if (TargetedPlayer)
			return;

		TargetedPlayer = Player;
		OnPlayerPerceived(Player);
	}
	else
	{
		OnPlayerLost(Stimulus.StimulusLocation);
	}
}

void AFretteEnemyCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

FVector AFretteEnemyCharacter::GetRandomPatrolPoint() const
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
		return FVector::ZeroVector;

	FNavLocation ResultLocation;

	const bool bFound = NavSystem->GetRandomPointInNavigableRadius(
		PatrolOrigin,
		PatrolRadius,
		ResultLocation
		);

	return bFound ? ResultLocation.Location : FVector::ZeroVector;
}

void AFretteEnemyCharacter::Die()
{
	Super::Die();

	if (!HasAuthority())
		return;

	OnDied.Broadcast(this);
	Multicast_HandleDeath(GetCharacterMovement()->Velocity);
}

void AFretteEnemyCharacter::Multicast_HandleDeath_Implementation(FVector FinalVelocity)
{
	Super::Multicast_HandleDeath_Implementation(FinalVelocity);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	GetMesh()->bPauseAnims = true;

	FTransform MeshWorldTransform = GetMesh()->GetComponentTransform();
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMesh()->SetWorldTransform(MeshWorldTransform);

	//TODO: Faire un parent bone string field qui permet de déterminer le bone à partir duquel le ragdoll doit être appliqué
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("hips"), true, true);
	GetMesh()->SetPhysicsLinearVelocity(FinalVelocity, false, FName("hips"));
}