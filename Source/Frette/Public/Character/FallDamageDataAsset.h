#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FallDamageDataAsset.generated.h"

class UGameplayEffect;

UCLASS()
class UFallDamageDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float MinFallHeight = 600.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> FallDamageEffect;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FRuntimeFloatCurve DamageCurve;
};