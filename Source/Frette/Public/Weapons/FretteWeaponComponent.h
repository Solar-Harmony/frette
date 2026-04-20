#pragma once

#include "CoreMinimal.h"
#include "FretteWeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShoot);

// Blueprint-side prefab for a weapon
UCLASS(DisplayName="Frette Weapon Component", meta=(BlueprintSpawnableComponent))
class UFretteWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Frette", meta=(MakeEditWidget=true))
	FVector ProjectilePosition;
	
	// Opportunity to play VFX, animations, etc.
	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable, Category="Frette")
	void Multicast_OnShootCosmeticEvent();
	
	UPROPERTY(BlueprintAssignable)
	FOnShoot OnShoot;
};

inline void UFretteWeaponComponent::Multicast_OnShootCosmeticEvent_Implementation()
{
	OnShoot.Broadcast();
}