#pragma once

#include "CoreMinimal.h"
#include "Actor/PickUpActor.h"
#include "FretteClue.generated.h"

/**
 * A pickable that provides a hint about a location in the world. 
 * Content is determined upon pickup, and may be one of:
 * - Flavor text with no gameplay information
 * - Knowledge of a landmark far from the main objective that contains useful loot, with directions
 * - Knowledge of a landmark near the main objective that helps narrow its location down
 */
UCLASS()
class AFretteClue : public APickUpActor
{
	GENERATED_BODY()

public:
	AFretteClue();
};