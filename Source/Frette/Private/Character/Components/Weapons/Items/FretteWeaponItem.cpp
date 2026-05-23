#include "Character/Components/Weapons/Items/FretteWeaponItem.h"
#include "Character/FretteBaseCharacter.h"
#include "GameFramework/Character.h"
#include "Inventory/FretteInventoryComponent.h"

void UFretteWeaponItem::OnEquipped()
{
	Super::OnEquipped();
	const AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());

	if (!Character->HasAuthority())
		return;
	
	const UFretteWeaponItemDataAsset* ItemData = Cast<UFretteWeaponItemDataAsset>(GetData());
	SpawnEquipmentActor(ItemData);
}

void UFretteWeaponItem::OnUnequipped()
{
	Super::OnUnequipped();
	
	const AFretteBaseCharacter* Character = Cast<AFretteBaseCharacter>(GetOwningInventory()->GetOwner());
	if (!Character->HasAuthority())
		return;
	
	if (SpawnedActor)
	{
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;
	}
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