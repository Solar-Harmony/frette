


#include "Actor/PickUpActor.h"

#include "Character/FrettePlayerCharacter.h"

APickUpActor::APickUpActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	StaticMesh->SetupAttachment(Root);
	
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(Root);
}

void APickUpActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemMesh)
	{
		StaticMesh->SetStaticMesh(ItemMesh);
		const FBoxSphereBounds Bounds = ItemMesh->GetBounds();
		const FVector CurrentSize = Bounds.BoxExtent * 2.0f;
		const float MaxDim = FMath::Max3(CurrentSize.X, CurrentSize.Y, CurrentSize.Z);
		const float ScaleFactor = (MaxDim > 0.0f) ? (Item_Size / MaxDim) : 1.0f;
		StaticMesh->SetRelativeScale3D(FVector(ScaleFactor));
		Sphere->SetSphereRadius(Item_Size / 2.f);
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::PickedUp);
	}
}

void APickUpActor::PickedUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!(OtherActor && OtherActor != this && OtherActor->IsA(AFrettePlayerCharacter::StaticClass())))
		return;
	
	OnPickedUp.Broadcast(OtherActor);
	
	if (bDestroyOnPickUp)
		Destroy();
}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	YawAngle += DeltaTime * Item_Rotation_Speed;
	YawAngle = FMath::Fmod(YawAngle, 360.f);
	
	ZPhase += DeltaTime * Item_Z_Mod_Speed;
	ZPhase = FMath::Fmod(ZPhase, 2.f * PI);
	
	const FRotator Rotator = FRotator(0.f, YawAngle, 0.f);
	Root->SetRelativeRotation(Rotator);
	Root->SetRelativeLocation(FVector(
		0.f, 
		0.f,
		Item_Z_Offset + (FMath::Sin(ZPhase) + 1.f) / 2.f * Item_Z_Mod_Range));
}

