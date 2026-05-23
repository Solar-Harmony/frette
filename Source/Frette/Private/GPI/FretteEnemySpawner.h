#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/FretteEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "FretteEnemySpawner.generated.h"

UCLASS()
class FRETTE_API AFretteEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AFretteEnemySpawner();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Frette")
	TObjectPtr<USphereComponent> SpawnZone;

	UPROPERTY(EditAnywhere, Category = "Frette")
	TSubclassOf<AFretteEnemyCharacter> EntityToSpawn;

	UPROPERTY(EditAnywhere, Category = "Frette")
	int32 MaxSpawnedEntities = 3;

	UPROPERTY(EditAnywhere, Category = "Frette")
	FVector2f SpawnTimerRandomInterval = FVector2f(5.0f, 20.0f);

	int RemainingSpawnedEntities = 0;

	void SpawnEntity();
	FVector GetRandomNavPoint() const;

	UFUNCTION()
	void OnEntityDied(AFretteEnemyCharacter* Entity);

	#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> Billboard;

	UPROPERTY(EditAnywhere, Category = "Frette|Editor")
	TObjectPtr<UTexture2D> BillboardIcon;
	#endif

};