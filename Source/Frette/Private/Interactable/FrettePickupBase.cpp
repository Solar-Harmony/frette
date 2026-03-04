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

void AFrettePickupBase::OnInteract_Internal()
{
	OnPickUp();
	
	if (bDestroyOnPickUp)
		Destroy();
}

void AFrettePickupBase::OnConstruction(const FTransform& Transform)
{
	require(ItemData, "An item pickup is missing item data.");
	
	ItemData->Mesh.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &AFrettePickupBase::OnItemMeshLoaded));
}

void AFrettePickupBase::OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject)
{
	UStaticMesh* Mesh = Cast<UStaticMesh>(LoadedObject);
	require(IsValid(Mesh), "Failed to load mesh for item '%s'.", GetNameSafe(ItemData))
	StaticMesh->SetStaticMesh(Mesh);
}
