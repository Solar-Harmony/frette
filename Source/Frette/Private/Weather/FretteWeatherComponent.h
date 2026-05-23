#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/FretteGameState.h"
#include "GameFramework/GameStateBase.h"
#include "Weather/FretteWeatherState.h"
#include "FretteWeatherComponent.generated.h"

class UFretteWeatherConfig;

USTRUCT(BlueprintType)
struct FRETTE_API FFretteActiveWeather
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFretteWeatherDataAsset> WeatherData = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	float TimeRemaining = 0.0f;
};

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteWeatherComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteWeatherComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UFretteWeatherConfig> Config;

	FFretteActiveWeather ActiveWeather;
	FFretteActiveWeather NextWeather;

	float ActiveToNextWeatherBlendingFactor = 0.0f;
	int CurrentTemperature = 0;

	FFretteActiveWeather ChooseNextWeather() const;
	void UpdateGameStateWeather() const;
};

static float GetSafeAmbientTemperature(UWorld* InWorld)
{
	if (!IsValid(InWorld))
		return 0.0f;

	AFretteGameState* GS = Cast<AFretteGameState>(InWorld->GetGameState());
	if (!IsValid(GS))
		return 0.0f;

	return GS->WeatherState.CurrentAmbientTemperature;
}

