#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
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
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* CollisionComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Projectile")
	UMaterialInterface* ImpactDecalMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Projectile")
	UNiagaraSystem* ImpactVFX;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Projectile")
	UNiagaraSystem* ImpactVFXFlesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Projectile")
	FVector ImpactDecalSize = FVector(5.0f, 25.0f, 25.0f);
	
	UPROPERTY(EditDefaultsOnly, meta= (ClampMin=0), Category="Frette|Projectile")
	int DamageAmount = 10;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ShowHitEffects(const FHitResult& Hit, const FVector& BoneVelocity, bool bHasBlood); 

	void ApplyDamage(UFretteBodyPartComponent* BodyPartComponent, FName HitBoneName) const;
};