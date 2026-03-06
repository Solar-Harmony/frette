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
	Interactable->bShowMessage = true;
	Interactable->bShowOutline = true;
	Interactable->OutlineColor = FColor::Blue;
}

void AFrettePickupBase::BeginPlay()
{
	Super::BeginPlay();
	Interactable->OnInteract.AddDynamic(this, &AFrettePickupBase::OnInteractDelegate);
}

void AFrettePickupBase::Server_OnInteract_Implementation(AFrettePlayerCharacter* Interactor)
{
	UFretteInventoryComponent* Inventory = Interactor->GetPlayerInventory();
	Inventory->AddItem_Implementation(this->ItemData);
	UFretteInventoryItem* AddedItem = Inventory->GetItem(Inventory->GetNumItems() - 1);
	
	OnPickUp(Interactor, AddedItem);
	
	if (bDestroyOnPickUp)
	{
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

void AFrettePickupBase::OnInteractDelegate(AFrettePlayerCharacter* Interactor)
{
	Server_OnInteract(Interactor);
	Client_OnInteract(Interactor);
}

void AFrettePickupBase::Client_OnInteract_Implementation(AFrettePlayerCharacter* Interactor)
{
	if (bDestroyOnPickUp)
	{
		Interactable->OnEndHover.Broadcast();
	}
}

void AFrettePickupBase::OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject) const
{
	UStaticMesh* Mesh = Cast<UStaticMesh>(LoadedObject);
	require(IsValid(Mesh), "Failed to load mesh for item '%s'.", GetNameSafe(ItemData))
	StaticMesh->SetStaticMesh(Mesh);
}
