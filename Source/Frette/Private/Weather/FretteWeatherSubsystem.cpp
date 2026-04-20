#include "Weather/FretteWeatherSubsystem.h"

#include "Core/FretteWorldSettings.h"
#include "Frette/Frette.h"

DEFINE_LOG_CATEGORY(LogFretteWeather);

void UFretteWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Config = CastChecked<AFretteWorldSettings>(GetWorld()->GetWorldSettings())->WeatherConfig;
	ActiveWeather = ChooseNextWeather();
	NextWeather = ChooseNextWeather();
	
	FRETTE_LOGC(Weather, Log, "Initial weather is '%s'.", *ActiveWeather.WeatherData->GetName());
}

bool UFretteWeatherSubsystem::IsTickable() const
{
	return Config->bUseDynamicWeather;
}

void UFretteWeatherSubsystem::Tick(float DeltaTime)
{
	ActiveWeather.TimeRemaining = FMath::Max(0.0f, ActiveWeather.TimeRemaining - DeltaTime);
	
	// are we done transitioning?
	if (ActiveWeather.TimeRemaining <= 0.0f)
	{
		ActiveWeather = NextWeather;
		NextWeather = ChooseNextWeather();
		ActiveToNextWeatherFac = 0.0f;
		FRETTE_LOGC(Weather, Log, "Transition to '%s' complete. Next weather will be '%s'.", *ActiveWeather.WeatherData->GetName(), *NextWeather.WeatherData->GetName());
	}
	// are we transitioning to the next weather?
	else if (ActiveWeather.TimeRemaining <= ActiveWeather.WeatherData->EndTransitionDuration)
	{
		const float RemainingRatio = ActiveWeather.TimeRemaining / ActiveWeather.WeatherData->EndTransitionDuration;
		ActiveToNextWeatherFac = 1.0f - FMath::Clamp(RemainingRatio, 0.0f, 1.0f);
	}
	else
	{
		ActiveToNextWeatherFac = 0.0f;
	}
	
	CurrentTemperature = FMath::Lerp(
		ActiveWeather.WeatherData->AmbientTemperature,
		NextWeather.WeatherData->AmbientTemperature,
		ActiveToNextWeatherFac);
	
	FRETTE_LOGC(Weather, Log, "Currently %f°C. Transitioning from '%s' (%f°C) to '%s' (%f°C)... %f%%.", (float)CurrentTemperature, *ActiveWeather.WeatherData->GetName(), ActiveWeather.WeatherData->AmbientTemperature, *NextWeather.WeatherData->GetName(), NextWeather.WeatherData->AmbientTemperature, ActiveToNextWeatherFac * 100.0f);
}

FFretteActiveWeather UFretteWeatherSubsystem::ChooseNextWeather() const
{
	UFretteWeatherDataAsset* WeatherData = Config->bUseDynamicWeather
		? Config->AvailableWeathers[FMath::RandRange(0, Config->AvailableWeathers.Num() - 1)]
		: Config->FixedWeather;

	FFretteActiveWeather WeatherInstance;
	const int TotalDuration = WeatherData->BaseDuration + FMath::RandRange(0, WeatherData->MaxAdditionalDuration) + WeatherData->EndTransitionDuration;
	WeatherInstance.TimeRemaining = TotalDuration;
	WeatherInstance.WeatherData = WeatherData;
	
	return WeatherInstance;
}