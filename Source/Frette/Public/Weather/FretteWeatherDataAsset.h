#pragma once

#include "CoreMinimal.h"
#include "FretteWeatherDataAsset.generated.h"

UCLASS()
class FRETTE_API UFretteWeatherDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName WeatherName;

	UPROPERTY(EditDefaultsOnly, meta=(Units="Celsius"))
	float AmbientTemperature;
	
	UPROPERTY(EditDefaultsOnly, meta=(Units="Seconds"))
	float BaseTransitionDuration;
};