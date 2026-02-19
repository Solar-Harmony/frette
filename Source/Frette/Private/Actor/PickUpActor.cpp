#include "Actor/PickUpActor.h"
#include "Character/FrettePlayerCharacter.h"
#include "PhysicsEngine/BodySetup.h"

APickUpActor::APickUpActor()
{
	PrimaryActorTick.bCanEverTick = false;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	SetRootComponent(OverlapSphere);
	OverlapSphere->SetSphereRadius((Item_Size / 2.f) * Sphere_Radius_Ratio);
	OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::PickedUp);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	StaticMesh->SetupAttachment(OverlapSphere);
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);
}

void APickUpActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IsValid(ItemMesh))
	{
		const UBodySetup* BodySetup = ItemMesh->GetBodySetup();
		const bool bHasCollision = IsValid(BodySetup) && BodySetup->AggGeom.GetElementCount() > 0;
		if (!bHasCollision)
		{
			UE_LOG(LogTemp, Error,
				TEXT("PickUpActor '%s': ItemMesh '%s' has no collision data. Assign a mesh with collision."),
				*GetName(), *ItemMesh->GetName());
			return;
		}

		StaticMesh->SetStaticMesh(ItemMesh);
		const FBoxSphereBounds Bounds = ItemMesh->GetBounds();
		const FVector CurrentSize = Bounds.BoxExtent * 2.0f;
		const float MaxDim = FMath::Max3(CurrentSize.X, CurrentSize.Y, CurrentSize.Z);
		const float ScaleFactor = (MaxDim > 0.0f) ? (Item_Size / MaxDim) : 1.0f;
		StaticMesh->SetRelativeScale3D(FVector(ScaleFactor));
		StaticMesh->SetRelativeLocation(FVector(0.f, 0.f, Item_Size / 2.f));
	}
}

void APickUpActor::PickedUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const bool bCanPickUp = IsValid(OtherActor)
		&& OtherActor != this
		&& OtherActor->IsA(AFrettePlayerCharacter::StaticClass());
	if (!bCanPickUp)
		return;
	
	OnPickedUp.Broadcast(OtherActor);
	
	if (bDestroyOnPickUp)
		Destroy();
}