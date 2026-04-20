#include "Weather/FretteWeatherSubsystem.h"

#include "Core/FretteWorldSettings.h"
#include "Frette/Frette.h"

DEFINE_LOG_CATEGORY(LogFretteWeather);

void UFretteWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (const AFretteWorldSettings* FretteWorldSettings = Cast<AFretteWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		Config = FretteWorldSettings->WeatherConfig;
	}

	if (Config == nullptr)
	{
		FRETTE_LOGC(Weather, Log, "No weather config set, weather won't work.");
		return;
	}

	ActiveWeather = ChooseNextWeather();
	NextWeather = ChooseNextWeather();
	
	FRETTE_LOGC(Weather, Log, "Initial weather is '%s'.", *ActiveWeather.WeatherData->GetName());
}

bool UFretteWeatherSubsystem::IsTickable() const
{
	return Config != nullptr && Config->bUseDynamicWeather;
}

void UFretteWeatherSubsystem::Tick(float DeltaTime)
{
	const bool bReport = CVarFretteWeatherReport.GetValueOnGameThread();
	
	ActiveWeather.TimeRemaining = FMath::Max(0.0f, ActiveWeather.TimeRemaining - DeltaTime);
	
	// are we done transitioning?
	if (ActiveWeather.TimeRemaining <= 0.0f)
	{
		ActiveWeather = NextWeather;
		NextWeather = ChooseNextWeather();
		ActiveToNextWeatherFac = 0.0f;

		if (bReport)
		{
			static uint64 Key = GetTypeHash(FName("FretteWeatherTransitionEnd"));
			const FString Msg = FretteFmt("Transition to '%s' complete. Next weather will be '%s'.", ActiveWeather.WeatherData, NextWeather.WeatherData);
			GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Green, Msg, false, FVector2D(3.0f, 3.0f));
		}
	}
	// are we transitioning to the next weather?
	else if (ActiveWeather.TimeRemaining <= ActiveWeather.WeatherData->EndTransitionDuration)
	{
		const float RemainingRatio = ActiveWeather.TimeRemaining / ActiveWeather.WeatherData->EndTransitionDuration;
		ActiveToNextWeatherFac = 1.0f - FMath::Clamp(RemainingRatio, 0.0f, 1.0f);

		if (bReport)
		{
			static uint64 Key = GetTypeHash(FName("FretteWeatherTransition"));
			const FString Msg = FretteFmt("Transitioning from '%s' (%d°C) to '%s' (%d°C)... %d%%.", ActiveWeather.WeatherData, ActiveWeather.WeatherData->AmbientTemperature, NextWeather.WeatherData, NextWeather.WeatherData->AmbientTemperature, FMath::CeilToInt(ActiveToNextWeatherFac * 100.0f));
			GEngine->AddOnScreenDebugMessage(Key, 0.5f, FColor::White, Msg, false, FVector2D(3.0f, 3.0f));
		}
	}
	else
	{
		ActiveToNextWeatherFac = 0.0f;

		if (bReport)
		{
			static uint64 Key = GetTypeHash(FName("FretteWeatherTransition"));
			const FString Msg = FretteFmt("Current weather is '%s', %ds left.", ActiveWeather.WeatherData, FMath::CeilToInt(ActiveWeather.TimeRemaining));
			GEngine->AddOnScreenDebugMessage(Key, 0.5f, FColor::Cyan, Msg, false, FVector2D(3.0f, 3.0f));
		}
	}
	
	// blend between current and next temps
	CurrentTemperature = FMath::Lerp(
		ActiveWeather.WeatherData->AmbientTemperature,
		NextWeather.WeatherData->AmbientTemperature,
		ActiveToNextWeatherFac);
	
	if (bReport)
	{
		static uint64 Key = GetTypeHash(FName("FretteWeatherThermometer"));
		const FString Msg = FretteFmt("[Currently %d°C]", CurrentTemperature);
		GEngine->AddOnScreenDebugMessage(Key, 0.5f, FColor::White, Msg, false, FVector2D(3.0f, 3.0f));
	}
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