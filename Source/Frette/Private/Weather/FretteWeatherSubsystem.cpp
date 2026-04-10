#include "Weather/FretteWeatherSubsystem.h"

#include "Core/FretteWorldSettings.h"

void UFretteWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	AFretteWorldSettings* Settings = CastChecked<AFretteWorldSettings>(GetWorld()->GetWorldSettings());
}