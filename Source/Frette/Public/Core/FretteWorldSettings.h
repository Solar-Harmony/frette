#pragma once

#include "CoreMinimal.h"
#include "FretteWorldSettings.generated.h"

UCLASS()
class FRETTE_API AFretteWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	// Weather to enable the whether system for this world.
	UPROPERTY(EditAnywhere, Category="Frette|Weather")
	bool bEnableWeather = true;
	
	UPROPERTY(EditAnywhere, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float MinTemperature = -50; // deg c
	
	UPROPERTY(EditAnywhere, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float MaxTemperature = 1500; // deg c
};
