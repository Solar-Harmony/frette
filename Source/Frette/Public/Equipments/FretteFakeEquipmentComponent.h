#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/FretteWeaponInstance.h"
#include "FretteFakeEquipmentComponent.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteFakeEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFretteFakeEquipmentComponent();
	void Equip(UFretteWeaponInstance* Equipment);
	void Unequip(UFretteWeaponInstance* Equipment);
	TObjectPtr<UFretteWeaponInstance> EquippedWeapon;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};