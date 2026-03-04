#pragma once

#include "CoreMinimal.h"
#include "FretteLandmark.generated.h"

/**
 * A point of interest that clues can point to.
 * All non-abstract POI instances in the level are cached on game start and can be accessed via the Frette Game Mode.
 */
UCLASS(Abstract)
class AFretteLandmark : public AActor
{
	GENERATED_BODY()

public:
	// The name of that point of interest. Will be displayed in clues.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frette")
	FText DisplayName;
	
	// Whether this landmark is near the main objective. 
	// This property is used by clues and is calculated by the Frette game mode on game start.
	// If true, this landmark is treated as a hint to the main objective's location.
	// If false, this landmark marks an unrelated point of interest, like valuable loot.
	UPROPERTY(BlueprintReadOnly)
	bool bIsNearMainObjective = false;
};