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

	UPROPERTY(EditAnywhere)
	int MaxAmmo = 30;

	int CurrentAmmo;

	UPROPERTY(EditAnywhere)
	int BulletPerCartridge;

	UPROPERTY(EditAnywhere)
	float MaxDamageRange;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve DistanceDamageFalloff;
};