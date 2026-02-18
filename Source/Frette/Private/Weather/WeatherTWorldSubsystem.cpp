


#include "WeatherTWorldSubsystem.h"

UWeatherDataAsset* UWeatherTWorldSubsystem::GetWeather()
{
	UE_LOG(LogTemp, Warning, TEXT("UWeatherTWorldSubsystem Get from BluePrint"));
	return CurrentWeather;
}

void UWeatherTWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("UWeatherTWorldSubsystem::Tick()"));
}

TStatId UWeatherTWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWeatherTWorldSubsystem, STATGROUP_Tickables);
}