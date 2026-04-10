#pragma once

#include "CoreMinimal.h"
#include "Weather/FretteWeatherDataAsset.h"
#include "FretteWorldSettings.generated.h"

UCLASS()
class FRETTE_API AFretteWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	// Weather to enable the whether system for this world.
	UPROPERTY(EditAnywhere, Category="Frette|Weather")
	bool bEnableWeather;
	
	UPROPERTY(EditAnywhere, Category="Frette|Weather", meta=(EditCondition="bEnableWeather", EditConditionHides))
	TArray<TObjectPtr<UFretteWeatherDataAsset>> Weathers;
}