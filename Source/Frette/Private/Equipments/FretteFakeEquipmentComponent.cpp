#include "Equipments/FretteFakeEquipmentComponent.h"

UFretteFakeEquipmentComponent::UFretteFakeEquipmentComponent()
{
	bReplicateUsingRegisteredSubObjectList = true;
	SetIsReplicated(true);
}

void UFretteFakeEquipmentComponent::Equip(UFretteWeaponInstance* Equipment)
{
	UFretteWeaponInstance* WeaponInstance = Cast<UFretteWeaponInstance>(Equipment);
	EquippedWeapon = WeaponInstance;
	EquippedWeapon->OnEquipped();

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(EquippedWeapon);
	}
}

void UFretteFakeEquipmentComponent::Unequip(UFretteWeaponInstance* Equipment)
{
	if (IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(Equipment);
	}

	Equipment->OnUnequipped();
	EquippedWeapon = nullptr;
}

void UFretteFakeEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

}