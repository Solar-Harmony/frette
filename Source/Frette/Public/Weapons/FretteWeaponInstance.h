#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/AbilitySetDataAsset.h"
#include "UObject/Object.h"
#include "FretteWeaponInstance.generated.h"

USTRUCT()
struct FFretteEquipmentActorToSpawn
{
	GENERATED_BODY()

	FFretteEquipmentActorToSpawn() {}

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere)
	FName AttachSocket;

	UPROPERTY(EditAnywhere)
	FTransform AttachTransform;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class UFretteWeaponInstance : public UObject
{
	GENERATED_BODY()

public:
	UFretteWeaponInstance();

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;

	UFUNCTION(BlueprintCallable)
	APawn* GetPawn() const;
	//~End of UObject interface
	UFUNCTION()
	virtual void OnEquipped();
	virtual void SpawnEquipmentActor();
	virtual void OnUnequipped();

	//Change pas at runtime
	UPROPERTY(EditDefaultsOnly)
	UAbilitySetDataAsset* AbilitySet;

	UPROPERTY(EditDefaultsOnly)
	FFretteEquipmentActorToSpawn EquipmentActor;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<AActor> SpawnedActor;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();
};