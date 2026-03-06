#pragma once

#include "CoreMinimal.h"
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
	FText GenerateClue(const AFrettePlayerCharacter* Interactor, const UFretteClueTemplateSet* Template, float DudClueChance);
	
protected:
	virtual void BeginPlay() override;

private:
	bool ShouldPickPrimaryClue() const;
	
	// Returns a random landmark, which can never be picked again (sampling without replacement).
	// @param bNearObjective Whether we pick from the landmarks near the treasure (primary, quest hints) or away (secondary, POIs)
	AFretteLandmark* GetRandomLandmark(bool bNearObjective);

	static FString DirVectorToCardinal(const FVector2D& Dir);
	
	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> NearLandmarks;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> FarLandmarks;
	
	int32 NumCluesPlaced = 0;
	int32 NumCluesFound = 0;
	int32 NumPrimaryCluesFound = 0;
	const float PrimaryCluesRatioTarget = 0.3f; // max % of clues that should be primary
	// PrimaryCluesRatioTarget starts at 0.0f, this is steepness of exponent curve to raise it
	const float PrimaryCluesRatioTargetRampSteepness = 0.25f;
};
