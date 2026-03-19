#pragma once

#include "CoreMinimal.h"
#include "FretteExtractionPoint.generated.h"

/**
 * If a player gets there with the main objective item in his inventory, they win the game.
 * Needs a Blueprint to configure collision for overlaps.
 */
UCLASS(Abstract)
class AFretteExtractionPoint : public AActor
{
	GENERATED_BODY()

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};