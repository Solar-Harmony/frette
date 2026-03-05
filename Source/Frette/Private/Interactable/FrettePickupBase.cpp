#include "Interactable/FrettePickupBase.h"
#include "Frette/Frette.h"
#include "Interactable/FretteInteractableComponent.h"

AFrettePickupBase::AFrettePickupBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);
	SetRootComponent(StaticMesh);
	
	Interactable = CreateDefaultSubobject<UFretteInteractableComponent>(TEXT("Interactable Component"));
	Interactable->OnInteract.AddDynamic(this, &AFrettePickupBase::OnInteract_Internal);
	Interactable->Mesh = StaticMesh;
	Interactable->bShowMessage = true;
	Interactable->bShowOutline = true;
	Interactable->OutlineColor = FColor::Blue;
}

void AFrettePickupBase::OnInteract_Internal(AFrettePlayerCharacter* Interactor)
{
	OnPickUp(Interactor);
	
	if (bDestroyOnPickUp)
		Destroy();
}

void AFrettePickupBase::OnConstruction(const FTransform& Transform)
{
	if (!IsValid(ItemData))
		return;
	
	require(!ItemData->Mesh.IsNull(), "Pickup actor '%s' has no mesh specified, so it will be invisible during play!", *GetName())
	
	// TODO: I've heard that LoadAsync has caveats but dont remember, should recheck when have time
	ItemData->Mesh.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &AFrettePickupBase::OnItemMeshLoaded));
}

void AFrettePickupBase::OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject)
{
	UStaticMesh* Mesh = Cast<UStaticMesh>(LoadedObject);
	require(IsValid(Mesh), "Failed to load mesh for item '%s'.", GetNameSafe(ItemData))
	StaticMesh->SetStaticMesh(Mesh);
}
