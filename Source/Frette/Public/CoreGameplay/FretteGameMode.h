#pragma once

#include "CoreMinimal.h"
#include "FretteClueGenerationDataAsset.h"
#include "FretteClueTemplateSet.h"
#include "CoreGameplay/FretteClue.h"
#include "GameFramework/GameModeBase.h"
#include "Weather/FretteWeatherComponent.h"
#include "FretteGameMode.generated.h"

class UFretteClueTemplateSet;
class AFrettePlayerCharacter;
class AFretteMainObjective;
class AFretteLandmark;

/**
 * The main game loop logic for Frette. Caches the actors and handles logic for the clue system.
 * Keep in mind the game mode runs only on the server!
 */
UCLASS()
class AFretteGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFretteGameMode();

	// Picks a clue, notifies clients
	FText GenerateClue(const AFrettePlayerCharacter* Interactor, const UFretteClueTemplateSet* Template);

	// Checks if player is close enough to collect the objective
	void ProbeForObjective(const AFrettePlayerCharacter* PlayerCharacter);

	// Checks if the player meets condition to win the game
	void CheckVictory(const AFrettePlayerCharacter* PlayerCharacter) const;

	bool IsGameEnded() const;

protected:
	virtual void BeginPlay() override;

	AFretteMainObjective* GetMainObjective() const { return MainObjective; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UFretteWeatherComponent> WeatherComponent;

	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> NearLandmarks;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> FarLandmarks;

	bool bPlayerCollectedObjective = false;

	int32 NumCluesPlaced = 0;
	int32 NumCluesFound = 0;
	int32 NumPrimaryCluesFound = 0;
	int32 NumDudCluesFound = 0;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteClueGenerationDataAsset> Cfg;
	
	float MinTimeBeforeNextPrimaryClue;
	float MaxTimeBeforeNextPrimaryClue;
	float LastPrimaryClueFoundTime = 0.0f;
	float GameStartTime = 0.0f;
	
	FText SpawnPointOfInterestReward(const AFretteLandmark* Landmark) const;
	
	void UpdateTimeBeforeNextPrimaryClue();

	EClueType PickNextClueType() const;

	// Returns a random landmark, which can never be picked again (sampling without replacement).
	// @param bNearObjective Whether we pick from the landmarks near the treasure (primary, quest hints) or away (secondary, POIs)
	AFretteLandmark* GetRandomLandmark(bool bNearObjective);

	virtual void RestartPlayer(AController* NewPlayer) override;

	UPROPERTY()
	TArray<TObjectPtr<AFrettePlayerCharacter>> Players;

	UFUNCTION()
	void OnPlayerDied(AFrettePlayerCharacter* PlayerCharacter);
	
	bool GetIfAllPlayersDead() const;
};