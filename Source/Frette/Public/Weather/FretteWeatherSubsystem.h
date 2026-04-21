#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Weather/FretteWeatherState.h"
#include "FretteWeatherSubsystem.generated.h"

class APostProcessVolume;

DECLARE_LOG_CATEGORY_EXTERN(LogFretteWeather, Log, All);

inline TAutoConsoleVariable CVarFretteWeatherReport(
		TEXT("Frette.Weather.Report"),
		false,
		TEXT("Displays information about weather on-screen."));

UCLASS()
class FRETTE_API UFretteWeatherSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void ApplyWeatherState(const FFretteWeatherState& State);
	
	UFUNCTION(BlueprintPure)
	int GetTemperature() const { return CachedTemperature; }
	
private:
	int CachedTemperature = 0;
	
	UPROPERTY()
	TWeakObjectPtr<APostProcessVolume> CachedPostProcessVolume;

	void FindPostProcessVolume();
};