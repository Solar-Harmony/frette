

#pragma once

#include "CoreMinimal.h"
#include "WeatherDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherTWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UWeatherTWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable) 
	UWeatherDataAsset* GetWeather();
	
	
protected: 
	UPROPERTY()
	UWeatherDataAsset* CurrentWeather;
	
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
};
