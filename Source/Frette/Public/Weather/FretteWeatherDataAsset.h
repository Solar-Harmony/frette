#pragma once

#include "CoreMinimal.h"
#include "FretteWeatherDataAsset.generated.h"

UCLASS()
class FRETTE_API UFretteWeatherDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Ambient temperature across the environment while this weather is fully active.
	UPROPERTY(EditDefaultsOnly, meta=(Units="Celsius"))
	int AmbientTemperature = 15;
	
	UPROPERTY(EditDefaultsOnly, meta=(Units="Seconds", ClampMin=5))
	int BaseDuration = 60;
	
	// TotalDuration = BaseDuration + rand(0, MaxRandomDurationOffset);
	UPROPERTY(EditDefaultsOnly, meta=(Units="Seconds", ClampMin=0))
	int MaxAdditionalDuration = 20;
	
	UPROPERTY(EditDefaultsOnly, meta=(Units="Seconds", ClampMin=1))
	int EndTransitionDuration = 10;
	
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0))
	float FogIntensity = 0.5f;
};

