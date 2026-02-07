#include "Weapons/FretteWeaponInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"

UFretteWeaponInstance::UFretteWeaponInstance() {}

UWorld* UFretteWeaponInstance::GetWorld() const
{
	if (const APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	return nullptr;
}

//Le Outer est assigné lors de l'instanciation du weapon présentement
//Il devrait être overrider quand un joueur le pick-up
APawn* UFretteWeaponInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UFretteWeaponInstance::OnEquipped()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Owner is ") + GetPawn()->GetName());
	UFretteAbilitySystemComponent* ASC = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	//Mis en If pour éviter de crash si on run un simulated proxy
	if (ASC)
		ASC->GrantAbilitiesFromAbilitySet(AbilitySet, this);
}

void UFretteWeaponInstance::OnUnequipped()
{
	UFretteAbilitySystemComponent* ASC = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	ensure(ASC);
	ASC->RemoveAbilitiesFromAbilitySet(AbilitySet);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Unequipped weapon from ") + GetPawn()->GetName());
}