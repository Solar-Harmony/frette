#include "AI/Spawner.h"

#include "NavigationSystem.h"
#include "Components/SphereComponent.h"

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnZone = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnZone"));
	RootComponent = SpawnZone;

	SpawnZone->SetHiddenInGame(true);
	SpawnZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	for (int i = 0; i < MaxSpawnedEntities; ++i)
	{
		SpawnEntity();
	}
}

void ASpawner::SpawnEntity()
{
	ensureAlwaysMsgf(EntityToSpawn, TEXT("EntityToSpawn is not set in %s. Please set it in the Blueprint."), *GetName());

	FVector SpawnLocation = GetRandomNavPoint();

	AFretteEnemyCharacter* Entity = GetWorld()->SpawnActorDeferred<AFretteEnemyCharacter>(
		EntityToSpawn,
		FTransform(FRotator::ZeroRotator, SpawnLocation),
		this
		);

	if (!Entity)
		return;

	Entity->PatrolOrigin = GetActorLocation();
	Entity->PatrolRadius = SpawnZone->GetScaledSphereRadius();
	Entity->OnDied.AddDynamic(this, &ASpawner::OnEntityDied);

	Entity->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));

	RemainingSpawnedEntities++;
}

void ASpawner::OnEntityDied(AFretteEnemyCharacter* entity)
{
	RemainingSpawnedEntities--;

	const FRandomStream Stream;
	const float RandomTimer = Stream.FRandRange(SpawnTimerRandomInterval.X, SpawnTimerRandomInterval.Y);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
		SpawnEntity();
	}, RandomTimer, false);
}

FVector ASpawner::GetRandomNavPoint() const
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!NavSystem)
		return FVector::ZeroVector;

	FNavLocation ResultLocation;
	const float Radius = SpawnZone->GetScaledSphereRadius();

	const bool bFound = NavSystem->GetRandomPointInNavigableRadius(
		GetActorLocation(),
		Radius,
		ResultLocation
		);

	return bFound ? ResultLocation.Location : FVector::ZeroVector;
}