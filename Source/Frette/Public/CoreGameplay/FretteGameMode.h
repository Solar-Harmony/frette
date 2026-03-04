#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FretteGameMode.generated.h"

class AFretteMainObjective;
class AFretteLandmark;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnClueActivated, FText);

/**
 * The main game loop logic for Frette. Caches the actors and handles logic for the clue system.
 */
UCLASS()
class AFretteGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FOnClueActivated OnClueActivated;
	
	UFUNCTION(BlueprintPure)
	AFretteMainObjective* GetMainObjective() const { return MainObjective; }
	
	UFUNCTION(BlueprintPure)
	int32 GetNumCluesFound() const { return NumCluesFound; }
	
	UFUNCTION(BlueprintPure)
	int32 GetNumCluesMax() const { return NumInitialClues; }
	
	// Returns a random landmark, which can never be picked again (sampling without replacement).
	// @param bNearObjective Whether we pick from the landmarks near the treasure (primary, quest hints) or away (secondary, POIs)
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Random Landmark"))
	AFretteLandmark* GetRandomLandmark(bool bNearObjective);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> NearLandmarks;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> FarLandmarks;
	
	int32 NumCluesFound = 0;
	int32 NumInitialClues = 0;
};
