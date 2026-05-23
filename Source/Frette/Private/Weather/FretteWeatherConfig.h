#pragma once

#include "CoreMinimal.h"
#include "FretteWeatherDataAsset.h"
#include "FretteWeatherConfig.generated.h"

UCLASS()
class FRETTE_API UFretteWeatherConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Frette|Weather")
	bool bUseDynamicWeather;
	
	UPROPERTY(EditAnywhere, Category="Frette|Weather", meta=(InlineEditConditionToggle))
	bool bOverrideWeatherDuration;
	
	UPROPERTY(EditAnywhere, Category="Frette|Weather", meta=(EditCondition="bOverrideWeatherDuration"))
	int OverriddenWeatherDuration;
	
	UPROPERTY(EditAnywhere, Category="Frette|Weather", meta=(EditCondition="!bUseDynamicWeather", EditConditionHides))
	TObjectPtr<UFretteWeatherDataAsset> FixedWeather;
	
	UPROPERTY(EditAnywhere, Category="Frette|Weather", meta=(EditCondition="bUseDynamicWeather", EditConditionHides))
	TArray<TObjectPtr<UFretteWeatherDataAsset>> AvailableWeathers;
};