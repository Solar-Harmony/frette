#pragma once

#include "CoreMinimal.h"
#include "Weather/FretteWeatherConfig.h"
#include "Weather/FretteWeatherDataAsset.h"
#include "FretteWorldSettings.generated.h"

UCLASS()
class FRETTE_API AFretteWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteWeatherConfig> WeatherConfig;
}