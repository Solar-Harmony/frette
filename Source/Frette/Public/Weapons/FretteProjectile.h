#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Components/SphereComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FretteProjectile.generated.h"

UCLASS()
class AFretteProjectile : public AActor
{
	GENERATED_BODY()

public:
	AFretteProjectile();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, meta= (ClampMin=0), Category="Frette|Damage")
	int DamageAmount = 10;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	void ApplyDamage(UFretteBodyPartComponent* BodyPartComponent, FName HitBoneName);

};