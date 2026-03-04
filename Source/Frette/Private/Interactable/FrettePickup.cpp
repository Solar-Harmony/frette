#include "Interactable/FrettePickup.h"

#include "Frette/Frette.h"
#include "Interactable/FretteInteractableComponent.h"

AFrettePickup::AFrettePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);
	SetRootComponent(StaticMesh);
	
	Interactable = CreateDefaultSubobject<UFretteInteractableComponent>(TEXT("Interactable Component"));
	Interactable->OnInteract.AddDynamic(this, &AFrettePickup::OnInteract_Internal);
	Interactable->Mesh = StaticMesh;
	Interactable->bShowMessage = true;
	Interactable->bShowOutline = true;
	Interactable->OutlineColor = FColor::Blue;
}

void AFrettePickup::OnInteract_Internal()
{
	OnInteract();
	
	if (bDestroyOnPickUp)
		Destroy();
}

void AFrettePickup::OnConstruction(const FTransform& Transform)
{
	require(ItemData, "An item pickup is missing item data.");
	
	ItemData->Mesh.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &AFrettePickup::OnItemMeshLoaded));
}

// TODO: Cpp: why does rider suggest this can be const?
void AFrettePickup::OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject)
{
	UStaticMesh* Mesh = Cast<UStaticMesh>(LoadedObject);
	require(IsValid(Mesh), "Failed to load mesh for item '%s'.", GetNameSafe(ItemData))
	StaticMesh->SetStaticMesh(Mesh);
}
