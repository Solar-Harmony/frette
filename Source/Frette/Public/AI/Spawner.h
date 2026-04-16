#pragma once

#include "CoreMinimal.h"
#include "Character/FretteEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class FRETTE_API ASpawner : public AActor
{
	GENERATED_BODY()

public:
	ASpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Frette")
	TObjectPtr<USphereComponent> SpawnZone;

	UPROPERTY(EditAnywhere, Category = "Frette")
	TSubclassOf<AFretteEnemyCharacter> EntityToSpawn;

	UPROPERTY(EditAnywhere, Category = "Frette|Spawner")
	int32 MaxSpawnedEntities = 3;

	UPROPERTY(EditAnywhere, Category = "Frette")
	FVector2f SpawnTimerRandomInterval = FVector2f(5.f, 20.f);

	UPROPERTY(EditAnywhere, Category = "Frette")
	TObjectPtr<USplineComponent> SpawnedEntitiesPatrolPath;

	int RemainingSpawnedEntities = 0;

	void SpawnEntity();

	UFUNCTION()
	void OnEntityDied(AFretteEnemyCharacter* entity);
	FVector GetRandomNavPoint() const;

};