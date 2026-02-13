#include "Weapons/FretteWeaponInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

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
	if (!GetPawn()->HasAuthority())
		return;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Owner is ") + GetPawn()->GetName());
	UFretteAbilitySystemComponent* ASC = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	SpawnEquipmentActor();
	//Mis en If pour éviter de crash si on run un simulated proxy
	if (ASC)
		ASC->GrantAbilitiesFromAbilitySet(AbilitySet, this);
}

void UFretteWeaponInstance::SpawnEquipmentActor()
{
	APawn* OwningPawn = GetPawn();

	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();

	if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = Char->GetMesh();
	}

	AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(EquipmentActor.ActorToSpawn, FTransform::Identity, OwningPawn);

	NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
	NewActor->SetActorRelativeTransform(EquipmentActor.AttachTransform);
	//On a pas de skeletal mesh pour le player pour l'instant
	NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform/*, EquipmentActor.AttachSocket*/);

	SpawnedActors.Add(NewActor);

}

void UFretteWeaponInstance::OnUnequipped()
{
	UFretteAbilitySystemComponent* ASC = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	ensure(ASC);
	ASC->RemoveAbilitiesFromAbilitySet(AbilitySet);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Unequipped weapon from ") + GetPawn()->GetName());
}

void UFretteWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SpawnedActors);
}