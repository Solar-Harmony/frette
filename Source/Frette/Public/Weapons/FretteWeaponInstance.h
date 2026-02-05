#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/AbilitySetDataAsset.h"
#include "UObject/Object.h"
#include "FretteWeaponInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class UFretteWeaponInstance : public UObject
{
	GENERATED_BODY()

public:
	UFretteWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	APawn* GetPawn() const;
	//~End of UObject interface

	virtual void OnEquipped();
	virtual void OnUnequipped();

	UPROPERTY(EditDefaultsOnly)
	UAbilitySetDataAsset* AbilitySet;
};