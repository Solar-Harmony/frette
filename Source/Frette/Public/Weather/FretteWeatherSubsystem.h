#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FretteWeatherSubsystem.generated.h"

class UFretteWeatherConfig;
class UFretteWeatherDataAsset;
class AFretteWorldSettings;

DECLARE_LOG_CATEGORY_EXTERN(LogFretteWeather, Log, All);

USTRUCT(BlueprintType)
struct FRETTE_API FFretteActiveWeather
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFretteWeatherDataAsset> WeatherData = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	float TimeRemaining = 0.0f;
};

UCLASS()
class FRETTE_API UFretteWeatherSubsystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	
	virtual TStatId GetStatId() const override { return TStatId(); }
	
	UFUNCTION(BlueprintPure)
	int GetTemperature() const { return CurrentTemperature; }
	
private:
	FFretteActiveWeather ActiveWeather;
	FFretteActiveWeather NextWeather;
	float ActiveToNextWeatherFac = 1.0f;
	
	int CurrentTemperature = 0;
	
	UPROPERTY()
	TObjectPtr<UFretteWeatherConfig> Config;
	
	FFretteActiveWeather ChooseNextWeather() const;
};