#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FretteGameMode.generated.h"

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
	UFUNCTION(BlueprintPure)
	AFretteMainObjective* GetMainObjective() const { return MainObjective; }
	
	UFUNCTION(BlueprintPure)
	int32 GetNumCluesFound() const { return NumCluesFound; }
	
	UFUNCTION(BlueprintPure)
	int32 GetNumCluesMax() const { return NumInitialClues; }
	
	// Picks a clue, notifies clients
	void GenerateClue(AFrettePlayerCharacter* Interactor, float DudClueChance, float Steepness, float Midpoint);
	
protected:
	virtual void BeginPlay() override;

private:
	bool ShouldPickPrimaryClue(float Steepness, float Midpoint) const;
	
	// Returns a random landmark, which can never be picked again (sampling without replacement).
	// @param bNearObjective Whether we pick from the landmarks near the treasure (primary, quest hints) or away (secondary, POIs)
	AFretteLandmark* GetRandomLandmark(bool bNearObjective);
	
	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> NearLandmarks;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> FarLandmarks;
	
	int32 NumCluesFound = 0;
	int32 NumInitialClues = 0;
};
