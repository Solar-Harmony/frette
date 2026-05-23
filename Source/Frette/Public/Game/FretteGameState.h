#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Weather/FretteWeatherState.h"
#include "FretteGameState.generated.h"

UENUM(BlueprintType)
enum class EGameOutcome : uint8
{
	InProgress,
	Victory,
	Defeat
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver, EGameOutcome, Outcome);

UCLASS()
class AFretteGameState : public AGameStateBase
{
	GENERATED_BODY()

	friend class AFretteGameMode;

public:
	FOnGameOver OnGameOver;

	UPROPERTY(ReplicatedUsing=OnRep_WeatherState)
	FFretteWeatherState WeatherState;

	UFUNCTION()
	void OnRep_WeatherState() const;

private:
	UPROPERTY(ReplicatedUsing=OnRep_GameOutcome)
	EGameOutcome GameOutcome = EGameOutcome::InProgress;

	UFUNCTION()
	void OnRep_GameOutcome() const
	{
		OnGameOver.Broadcast(GameOutcome);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, GameOutcome);
		DOREPLIFETIME(ThisClass, WeatherState);
	}
};