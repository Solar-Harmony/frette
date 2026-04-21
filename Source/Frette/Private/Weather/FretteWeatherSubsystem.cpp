#include "Weather/FretteWeatherSubsystem.h"

#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"

DEFINE_LOG_CATEGORY(LogFretteWeather);

void UFretteWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FindPostProcessVolume();
}

void UFretteWeatherSubsystem::Deinitialize()
{
	CachedPostProcessVolume.Reset();
	Super::Deinitialize();
}

void UFretteWeatherSubsystem::FindPostProcessVolume()
{
	// TODO(weather): use frette post subsystem
	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		if (It->bUnbound)
		{
			CachedPostProcessVolume = *It;
			break;
		}
	}
}

void UFretteWeatherSubsystem::ApplyWeatherState(const FFretteWeatherState& State)
{
	CachedTemperature = State.CurrentAmbientTemperature;

	if (CachedPostProcessVolume.IsValid() && State.ActiveWeatherData && State.NextWeatherData)
	{
		float FogIntensity = FMath::Lerp(
			State.ActiveWeatherData->FogIntensity,
			State.NextWeatherData->FogIntensity,
			State.ActiveToNextWeatherBlendingFactor);
	}
}