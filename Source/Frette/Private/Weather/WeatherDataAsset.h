

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeatherDataAsset.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeatherType : uint8
{
	Calm,
	Snowy,
	Cloudy,
	CloudySnowy,
	LightSnowstorm,
	Snowstorm,
	Blizzard
};


UCLASS(BlueprintType)
class UWeatherDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Weather")
	EWeatherType WeatherType;

	UPROPERTY(EditAnywhere, Category="Weather")
	float TemperatureCelsius;

	UPROPERTY(EditAnywhere, Category="Weather")
	float WindSpeedKmh;

	UPROPERTY(EditAnywhere, Category="Weather")
	UMaterialInterface* SkyMaterial;

	UPROPERTY(EditAnywhere, Category="Weather")
	USoundBase* OutsideSound;
	
	UPROPERTY(EditAnywhere, Category="Weather")
	USoundBase* InsideSound;
	
	
	UFUNCTION(BlueprintCallable) void GetTemperature();
};
