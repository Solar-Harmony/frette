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
	int32 GetNumCluesFound() const { return NumCluesDiscovered; }
	
	UFUNCTION(BlueprintPure)
	int32 GetNumCluesMax() const { return NumCluesGenerated; }
	
	// Picks a clue, notifies clients
	FText GenerateClue(const AFrettePlayerCharacter* Interactor, float DudClueChance);
	
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
	
	int32 NumCluesDiscovered = 0;
	int32 NumNearCluesGenerated = 0;
	int32 NumCluesGenerated = 0;
};
