#pragma once

#include "CoreMinimal.h"
#include "GPI/Interactable/FrettePickupBase.h"
#include "Game/Clues/FretteClueItem.h"
#include "FretteClue.generated.h"

class UFretteClueTemplateSet;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frette")
	TObjectPtr<UFretteClueTemplateSet> ClueTemplate;
	
	virtual void OnPickUp_Implementation(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem) override;
};