#include "Inventory/Items/Impl/FretteWeaponItem.h"

#include "GameFramework/Character.h"
#include "Inventory/FretteInventoryComponent.h"

void UFretteWeaponItem::OnEquipped()
{
	Super::OnEquipped();
	const UFretteWeaponItemDataAsset* ItemData = Cast<UFretteWeaponItemDataAsset>(GetData());
	SpawnEquipmentActor(ItemData);
}

void UFretteWeaponItem::SpawnEquipmentActor(const UFretteWeaponItemDataAsset* ItemData)
{
	APawn* OwningPawn = Cast<APawn>(GetOwningInventory()->GetOwner());

	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();

	if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = Char->GetMesh();
	}

	AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ItemData->ActorToSpawn, FTransform::Identity, OwningPawn);

	NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
	NewActor->SetActorRelativeTransform(ItemData->AttachTransform);
	NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, ItemData->AttachSocket);

	SpawnedActor = NewActor;
}