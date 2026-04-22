#include "Weather/FretteWeatherComponent.h"
#include "Core/FretteWorldSettings.h"
#include "CoreGameplay/FretteGameState.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Frette/Frette.h"
#include "Weather/FretteWeatherConfig.h"
#include "Weather/FretteWeatherSubsystem.h"

UFretteWeatherComponent::UFretteWeatherComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFretteWeatherComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AFretteWorldSettings* FretteWorldSettings = Cast<AFretteWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		Config = FretteWorldSettings->WeatherConfig;
	}

	if (Config == nullptr)
	{
		PrimaryComponentTick.bCanEverTick = false;
		FRETTE_LOGC(Weather, Log, "No weather config set, weather won't work.");
		return;
	}

	PrimaryComponentTick.bCanEverTick = Config->bUseDynamicWeather;

	ActiveWeather = ChooseNextWeather();
	NextWeather = ChooseNextWeather();

	if (ActiveWeather.WeatherData)
	{
		CurrentTemperature = ActiveWeather.WeatherData->AmbientTemperature;
	}

	FRETTE_LOGC(Weather, Log, "Initial weather is '%s'.", ActiveWeather.WeatherData);

	UpdateGameStateWeather();

	SetComponentTickEnabled(true);
}

void UFretteWeatherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bReport = CVarFretteWeatherReport.GetValueOnGameThread();

	ActiveWeather.TimeRemaining = FMath::Max(0.0f, ActiveWeather.TimeRemaining - DeltaTime);

	// are we done transitioning?
	if (ActiveWeather.TimeRemaining <= 0.0f)
	{
		ActiveWeather = NextWeather;
		NextWeather = ChooseNextWeather();
		ActiveToNextWeatherBlendingFactor = 0.0f;

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
		ActiveToNextWeatherBlendingFactor = 1.0f - FMath::Clamp(RemainingRatio, 0.0f, 1.0f);

		if (bReport)
		{
			static uint64 Key = GetTypeHash(FName("FretteWeatherTransition"));
			const FString Msg = FretteFmt("Transitioning from '%s' (%d°C) to '%s' (%d°C)... %d%%.", ActiveWeather.WeatherData, ActiveWeather.WeatherData->AmbientTemperature, NextWeather.WeatherData, NextWeather.WeatherData->AmbientTemperature, FMath::CeilToInt(ActiveToNextWeatherBlendingFactor * 100.0f));
			GEngine->AddOnScreenDebugMessage(Key, 0.5f, FColor::White, Msg, false, FVector2D(3.0f, 3.0f));
		}
	}
	else
	{
		ActiveToNextWeatherBlendingFactor = 0.0f;

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
		ActiveToNextWeatherBlendingFactor);

	if (bReport)
	{
		static uint64 Key = GetTypeHash(FName("FretteWeatherThermometer"));
		const FString Msg = FretteFmt("[Currently %d°C]", CurrentTemperature);
		GEngine->AddOnScreenDebugMessage(Key, 0.5f, FColor::White, Msg, false, FVector2D(3.0f, 3.0f));
	}
}

FFretteActiveWeather UFretteWeatherComponent::ChooseNextWeather() const
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

void UFretteWeatherComponent::UpdateGameStateWeather() const
{
	AFretteGameState* GameState = GetWorld()->GetGameState<AFretteGameState>();
	check(GameState != nullptr);

	FFretteWeatherState NewState;
	NewState.ActiveWeatherData = ActiveWeather.WeatherData;
	NewState.NextWeatherData = NextWeather.WeatherData;
	NewState.ActiveToNextWeatherBlendingFactor = ActiveToNextWeatherBlendingFactor;
	NewState.CurrentAmbientTemperature = CurrentTemperature;

	if (GameState->WeatherState != NewState)
	{
		GameState->WeatherState = NewState;

		// Listen server: OnRep is not automatically called on server
		if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_Standalone)
		{
			GameState->OnRep_WeatherState();
		}
	}
}