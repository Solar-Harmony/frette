#pragma once

#include "CoreMinimal.h"
#include "Interactable/FrettePickupBase.h"
#include "FretteClue.generated.h"

/**
 * A pickable that provides a hint about a location in the world. 
 * Needs a base Blueprint so the default properties can be configured.
 * Content is determined upon pickup, and may be one of:
 * - Flavor text with no gameplay information
 * - Knowledge of a landmark far from the main objective that contains useful loot, with directions
 * - Knowledge of a landmark near the main objective that helps narrow its location down
 */
UCLASS(Abstract)
class AFretteClue : public AFrettePickupBase
{
	GENERATED_BODY()

protected:
	// Gating probability of a clue being a flavor-only, lore text. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frette")
	float DudClueChance = 0.1f;
	
	// Parameter for the probability of getting a primary clue depending on the number of clues gotten so far.
	// A lower value means it takes fewer clues before that probability increases by a lot. 
	// A higher value means it takes more clues before we see a steep increase in probability.
	// Controls the horizontal shift of the S-curve (normalized sigmoid function).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frette")
	float Midpoint = 0.5f;
	
	// Parameter for the probability of getting a primary clue depending on the number of clues gotten so far.
	// A lower value means that probability increases more gradually with the number of clues.
	// A higher value means that probability increases more suddenly around the midpoint.
	// Controls the smoothness of the S-curve (normalized sigmoid function).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frette")
	float Steepness = 10.0f;
	
	virtual void OnPickUp_Implementation() override;
	
private:
	bool ShouldGivePrimaryClue(int NumCluesFound, int NumCluesTotal) const;
};