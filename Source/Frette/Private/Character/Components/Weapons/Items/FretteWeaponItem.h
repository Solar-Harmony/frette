#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "UObject/Object.h"
#include "FretteWeaponItem.generated.h"

class UFretteWeaponItemDataAsset;

UCLASS(Abstract, BlueprintType)
class UFretteWeaponItem : public UFretteSlottableItem
{
	GENERATED_BODY()
	FRETTE_ITEM_DATA_GETTER(UFretteWeaponItemDataAsset)

public:
	UFUNCTION()
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<AActor> SpawnedActor;

protected:
	virtual void SpawnEquipmentActor(const UFretteWeaponItemDataAsset* ItemData);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, SpawnedActor);
	}
};

UCLASS(Abstract, BlueprintType)
class UFretteWeaponItemDataAsset : public UFretteSlottableItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere)
	FName AttachSocket;

	UPROPERTY(EditAnywhere)
	FTransform AttachTransform;

	virtual const UClass* GetRuntimeItemClass() const override { return UFretteWeaponItem::StaticClass(); }
};