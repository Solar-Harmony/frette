#pragma once

#include "CoreMinimal.h"
#include "Weapons/FretteWeaponInstance.h"
#include "FretteRangedWeaponInstance.generated.h"

class AFretteProjectile;
/**
 * 
 */
UCLASS()
class UFretteRangedWeaponInstance : public UFretteWeaponInstance
{
	GENERATED_BODY()

public:
	UFretteRangedWeaponInstance();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFretteProjectile> ProjectileType;

	int GetCurrentAmmo() const { return CurrentClipAmmo; }

	void UseAmmo();

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve DistanceDamageFalloff;

private:
	int MaxClipAmmo = 30;

	//Change at runtime
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ClipAmmo)
	int CurrentClipAmmo;

	UFUNCTION()
	virtual void OnRep_ClipAmmo(int OldPrimaryClipAmmo);
};