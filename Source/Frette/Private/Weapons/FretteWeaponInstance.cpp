#include "Weapons/FretteWeaponInstance.h"

UFretteWeaponInstance::UFretteWeaponInstance(const FObjectInitializer& ObjectInitializer) {}

UWorld* UFretteWeaponInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	return nullptr;
}

APawn* UFretteWeaponInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UFretteWeaponInstance::OnEquipped()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Owner is ") + GetPawn()->GetName());
	//Ajoute les abiletées au character qui l'a équiper
}

void UFretteWeaponInstance::OnUnequipped()
{
	//Retire les abiletées
}