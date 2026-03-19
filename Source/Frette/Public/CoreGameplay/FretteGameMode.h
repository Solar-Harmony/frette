#pragma once

#include "CoreMinimal.h"
#include "FretteClueTemplateSet.h"
#include "CoreGameplay/FretteClue.h"
#include "GameFramework/GameModeBase.h"
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
	UFUNCTION(BlueprintPure)
	AFretteMainObjective* GetMainObjective() const { return MainObjective; }
	
	// Picks a clue, notifies clients
	FText GenerateClue(const AFrettePlayerCharacter* Interactor, const UFretteClueTemplateSet* Template);
	
protected:
	virtual void BeginPlay() override;

private:
	EClueType PickNextClueType() const;
	
	// Returns a random landmark, which can never be picked again (sampling without replacement).
	// @param bNearObjective Whether we pick from the landmarks near the treasure (primary, quest hints) or away (secondary, POIs)
	AFretteLandmark* GetRandomLandmark(bool bNearObjective);
	
	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> NearLandmarks;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> FarLandmarks;
	
	int32 NumCluesPlaced = 0;
	int32 NumCluesFound = 0;
	int32 NumPrimaryCluesFound = 0;
	int32 NumDudCluesFound = 0;
	const float PrimaryCluesRatioTarget = 0.3f;
	const float DudClueRatioTarget = 0.1f;
	// Both ratio targets start at 0.0, this is the steepness of the exponent curve ramping them up
	const float ClueRatioRampSteepness = 0.25f;
};
