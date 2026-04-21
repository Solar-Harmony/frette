#pragma once

#include "CoreMinimal.h"
#include "Weather/FretteWeatherDataAsset.h"
#include "FretteWeatherState.generated.h"

USTRUCT(BlueprintType)
struct FRETTE_API FFretteWeatherState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFretteWeatherDataAsset> ActiveWeatherData = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFretteWeatherDataAsset> NextWeatherData = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	int CurrentAmbientTemperature = 0;
	
	UPROPERTY()
	float ActiveToNextWeatherBlendingFactor = 0.0f;

	bool operator==(const FFretteWeatherState& Other) const
	{
		return ActiveWeatherData == Other.ActiveWeatherData
			&& NextWeatherData == Other.NextWeatherData 
			&& FMath::IsNearlyEqual(ActiveToNextWeatherBlendingFactor, Other.ActiveToNextWeatherBlendingFactor) 
			&& CurrentAmbientTemperature == Other.CurrentAmbientTemperature;
	}

	bool operator!=(const FFretteWeatherState& Other) const
	{
		return !(*this == Other);
	}
};
