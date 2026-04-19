#include "Weapons/FretteProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Components/BodyPart/FretteBodyPartTags.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "Util/FretteCollisionChannels.h"

AFretteProjectile::AFretteProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	AActor::SetReplicateMovement(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	//Doit ignorer le collider du cylindre du joueur
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	//Mais pas les colliders des meshes
	CollisionComponent->SetCollisionResponseToChannel(ECC_CharacterMesh, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	RootComponent = CollisionComponent;
}

void AFretteProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AFretteProjectile::OnHit);
	}
}

void AFretteProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority() || !OtherActor)
	{
		Destroy();
		return;
	}

	UFretteBodyPartComponent* BodyPart = OtherActor->GetComponentByClass<UFretteBodyPartComponent>();
	if (!BodyPart)
	{
		Destroy();
		return;
	}

	USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(Hit.GetComponent());
	if (!SkelMesh || !SkelMesh->GetPhysicsAsset() || Hit.Item == INDEX_NONE)
	{
		Destroy();
		return;
	}
	
	const USkeletalBodySetup* HitBody = SkelMesh->GetPhysicsAsset()->SkeletalBodySetups[Hit.Item];
	const FName HitPhysBone = HitBody->BoneName;
	const FVector BoneVelocity = SkelMesh->GetPhysicsLinearVelocity(HitPhysBone);
	
	Multicast_ShowHitEffects(Hit, BoneVelocity);

	ApplyDamage(BodyPart, HitPhysBone);
	
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorHiddenInGame(true);
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}
	SetLifeSpan(1.0f);
}

void AFretteProjectile::Multicast_ShowHitEffects_Implementation(const FHitResult& Hit, FVector BoneVelocity)
{
	if (IsValid(ImpactDecalMaterial))
	{
		UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			ImpactDecalMaterial,
			ImpactDecalSize,            
			Hit.ImpactPoint + Hit.ImpactNormal * 1.0f,
			Hit.ImpactNormal.Rotation(),
			0.0f
		);
	}

	if (IsValid(ImpactVFX))
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactVFX,
			Hit.ImpactPoint,
			FRotator::ZeroRotator,
			 FVector(1.0f),
			true,
			true,
			ENCPoolMethod::AutoRelease
		);
		
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariableVec3("User.ImpactNormal", Hit.ImpactNormal);
			NiagaraComponent->SetVariableVec3("User.BoneVelocity", BoneVelocity);
		}
	}
}

void AFretteProjectile::ApplyDamage(UFretteBodyPartComponent* BodyPartComponent, const FName HitBoneName) const
{
	BodyPartComponent->AddValueFromBoneName(HitBoneName, -DamageAmount, TAG_BodyPartValues_Health);
}