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
	// Name as used in clues (e.g. mine de cuivre). Keep in mind clues refer to the area before it was abandoned.
	UPROPERTY(EditAnywhere, Category = "Frette")
	FText DisplayName;
	
	// Short answer to the question "This {DisplayName} is <what>?" (e.g. essentielle pour l'économie de la région). Used in clues.
	UPROPERTY(EditAnywhere, Category = "Frette", meta=(MultiLine = true))
	FText Description;
	
	// One thing that can be found at the location of this landmark (e.g. my hidden cache of loot). Used by clues.
	UPROPERTY(EditAnywhere, Category = "Frette", meta=(MultiLine = true))
	FText ThingOfInterest;
	
	// Whether this landmark is near the main objective. 
	// This property is used by clues and is calculated by the Frette game mode on game start.
	// If true, this landmark is treated as a hint to the main objective's location.
	// If false, this landmark marks an unrelated point of interest, like valuable loot.
	UPROPERTY(BlueprintReadOnly)
	bool bIsNearMainObjective = false;
};