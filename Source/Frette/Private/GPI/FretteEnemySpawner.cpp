#include "GPI/FretteEnemySpawner.h"
#include "NavigationSystem.h"
#include "Components/BillboardComponent.h"
#include "Frette.h"

AFretteEnemySpawner::AFretteEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnZone = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnZone"));
	RootComponent = SpawnZone;

	SpawnZone->SetHiddenInGame(true);
	SpawnZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	#if WITH_EDITORONLY_DATA
	Billboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));

	if (Billboard)
	{
		Billboard->SetupAttachment(RootComponent);
	}
	#endif
}

void AFretteEnemySpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	#if WITH_EDITORONLY_DATA
	if (Billboard)
	{
		if (BillboardIcon)
		{
			Billboard->SetSprite(BillboardIcon);
		}
		Billboard->SetRelativeLocation(FVector(0.f, 0.f, SpawnZone->GetUnscaledSphereRadius()));
	}
	#endif
}

void AFretteEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	unless(NavSystem != nullptr, "Missing NavMeshBoundsVolume in the level. Frette enemy spawner will not work.")
		return;

	for (int i = 0; i < MaxSpawnedEntities; ++i)
	{
		SpawnEntity();
	}
}

void AFretteEnemySpawner::SpawnEntity()
{
	unless(EntityToSpawn, "EntityToSpawn is not set in %s. Please set it in the Blueprint.", *GetName())
		return;

	const FVector SpawnLocation = GetRandomNavPoint();

	AFretteEnemyCharacter* Entity = GetWorld()->SpawnActorDeferred<AFretteEnemyCharacter>(
		EntityToSpawn,
		FTransform(FRotator::ZeroRotator, SpawnLocation),
		this
		);

	if (!Entity)
		return;

	Entity->PatrolOrigin = GetActorLocation();
	Entity->PatrolRadius = SpawnZone->GetScaledSphereRadius();
	Entity->OnDied.AddDynamic(this, &AFretteEnemySpawner::OnEntityDied);

	Entity->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));

	RemainingSpawnedEntities++;
}

void AFretteEnemySpawner::OnEntityDied(AFretteEnemyCharacter* Entity)
{
	RemainingSpawnedEntities--;

	const float RandomTimer = FMath::RandRange(SpawnTimerRandomInterval.X, SpawnTimerRandomInterval.Y);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
		SpawnEntity();
	}, RandomTimer, false);
}

FVector AFretteEnemySpawner::GetRandomNavPoint() const
{
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

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