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
	// Weather to enable the whether system for this world.
	UPROPERTY(EditAnywhere, Category="Frette|Weather")
	bool bEnableWeather = true;
	
	UPROPERTY(EditAnywhere, Category="Frette|Weather")
	TObjectPtr<UFretteWeatherConfig> WeatherConfig;
	
	UPROPERTY(EditAnywhere, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float MinTemperature = -50; // deg c
	
	UPROPERTY(EditAnywhere, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float MaxTemperature = 1500; // deg c
	
	UPROPERTY(EditAnywhere, Category="Frette|Temperature", meta = (Units = "Seconds"))
	float TimeBeforeTemperatureUpdates = 0.5;
};