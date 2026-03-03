#pragma once

#include "CoreMinimal.h"
#include "FrettePOI.generated.h"

/**
 * A point of interest actor used by the clue system as a potential meaningful location to find the treasure.
 * All non-abstract POI instances in the level are cached on game start and can be accessed via UFrettePOISubsystem.
 */
UCLASS(Abstract)
class AFrettePOI : public AActor
{
	GENERATED_BODY()

public:
	// The name of that point of interest. Will be displayed in clues.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frette")
	FText DisplayName;

	// Whether this POI is inside the "near objective" radius of the main objective,
	// and should be pointed at by clues in the world
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsPrimaryPOI = false;
};