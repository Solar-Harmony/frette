#include "Interactable/FrettePickupBase.h"

#include "Character/FrettePlayerCharacter.h"
#include "Frette/Frette.h"
#include "Interactable/FretteInteractableComponent.h"
#include "Inventory/FretteInventoryComponent.h"

AFrettePickupBase::AFrettePickupBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

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
	require(HasAuthority(), "Pickup interact must happen on server.");
		
	OnPickUp(Interactor);
	
	UFretteInventoryComponent* Inventory = Interactor->GetPlayerInventory();
	Inventory->AddItem(this->ItemData);
	
	if (bDestroyOnPickUp)
	{
		Interactable->OnEndHover.Broadcast();
		Destroy();
	}
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
