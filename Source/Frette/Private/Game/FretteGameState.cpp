#include "Game/FretteGameState.h"
#include "Engine/World.h"
#include "Weather/FretteWeatherSubsystem.h"

void AFretteGameState::OnRep_WeatherState() const
{
	GetWorld()->GetSubsystem<UFretteWeatherSubsystem>()->ApplyWeatherState(WeatherState);
}

