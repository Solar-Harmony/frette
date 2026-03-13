#include "Equipments/FretteFakeEquipmentComponent.h"

#include "Engine/ActorChannel.h"

UFretteFakeEquipmentComponent::UFretteFakeEquipmentComponent()
{
	SetIsReplicated(true);
}

void UFretteFakeEquipmentComponent::Equip(UFretteWeaponInstance* Equipment)
{
	EquippedWeapon = Equipment;
	EquippedWeapon->OnEquipped();
}

void UFretteFakeEquipmentComponent::Unequip(UFretteWeaponInstance* Equipment)
{
	Equipment->OnUnequipped();
	EquippedWeapon = nullptr;
}

bool UFretteFakeEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (EquippedWeapon)
	{
		bWroteSomething |= Channel->ReplicateSubobject(EquippedWeapon, *Bunch, *RepFlags);
	}

	return bWroteSomething;
}

void UFretteFakeEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

}