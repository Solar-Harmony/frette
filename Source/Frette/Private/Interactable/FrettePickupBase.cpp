#include "Interactable/FrettePickupBase.h"

#include "Character/FrettePlayerCharacter.h"
#include "Frette/Frette.h"
#include "Interactable/FretteInteractableComponent.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteStackableItem.h"

AFrettePickupBase::AFrettePickupBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
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
	UFretteInventoryItem* AddedItem = nullptr;
	UFretteInventoryComponent* Inventory = Interactor->GetPlayerInventory();
	
	if (const auto* StackData = Cast<UFretteStackableItemDataAsset>(this->ItemData))
	{
		AddedItem = Inventory->GetFirstItemFromAsset(StackData->GetClass());
		if (AddedItem != nullptr)
		{
			UFretteStackableItem* Stack = Cast<UFretteStackableItem>(AddedItem);
			Stack->Quantity += StackData->DefaultQuantity; // TODO: prolly not always what we want
			Inventory->ChangeItem_Implementation(Stack);
		}
	}
	
	if (AddedItem == nullptr)
	{
		Inventory->AddItem_Implementation(this->ItemData);
		AddedItem = Inventory->GetItem(Inventory->GetNumItems() - 1);		
	}
	
	OnPickUp(Interactor, AddedItem);
	
	if (bDestroyOnPickUp)
	{
		Destroy();
	}
}

void AFrettePickupBase::OnConstruction(const FTransform& Transform)
{
	UpdateVisuals();
}

void AFrettePickupBase::UpdateVisuals()
{
	if (!IsValid(ItemData))
		return;
	
	precondition(!ItemData->Mesh.IsNull(), "Pickup actor '%s' has no mesh specified, so it will be invisible during play!", *GetName())
	
	Interactable->Message = FText::Format(NSLOCTEXT("Frette", "PickUpMessage", "[E] pick up {0}"), ItemData->DisplayName);
	
	// TODO: I've heard that LoadAsync has caveats but dont remember, should recheck when have time
	ItemData->Mesh.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &AFrettePickupBase::OnItemMeshLoaded));
}

void AFrettePickupBase::OnRep_ItemData()
{
	UpdateVisuals();
}

void AFrettePickupBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AFrettePickupBase, ItemData);
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
	precondition(IsValid(Mesh), "Failed to load mesh for item '%s'.", GetNameSafe(ItemData));
	
	StaticMesh->SetStaticMesh(Mesh);	
	StaticMesh->SetSimulatePhysics(bHasPhysics);
	
	if (bHasPhysics)
	{
		StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		StaticMesh->SetEnableGravity(true);
		StaticMesh->SetMassOverrideInKg(NAME_None, 100.0f, true);
	}
}
