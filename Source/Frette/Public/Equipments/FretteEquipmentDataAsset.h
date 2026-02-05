#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapons/FretteWeaponInstance.h"
#include "FretteEquipmentDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FRETTE_API UFretteEquipmentDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFretteWeaponInstance> InstanceType;
};