#include "Weapons/FretteProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Components/BodyPart/FretteBodyPartTags.h"
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
	ApplyDamage(BodyPart, HitPhysBone);
	Destroy();
}

void AFretteProjectile::ApplyDamage(UFretteBodyPartComponent* BodyPartComponent, const FName HitBoneName)
{
	BodyPartComponent->AddValueFromBoneName(HitBoneName, -DamageAmount, TAG_BodyPartValues_Health);
}