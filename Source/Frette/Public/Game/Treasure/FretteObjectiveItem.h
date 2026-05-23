#pragma once

#include "CoreMinimal.h"
#include "Character/Player/FrettePlayerCharacter.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "Character/Player/FrettePlayerController.h"
#include "FretteObjectiveItem.generated.h"

class UFretteObjectiveItemDataAsset;

UCLASS(BlueprintType)
class UFretteObjectiveItem : public UFretteSlottableItem
{
	GENERATED_BODY()
	FRETTE_ITEM_DATA_GETTER(UFretteObjectiveItemDataAsset)
};

UCLASS()
class UFretteObjectiveItemDataAsset : public UFretteSlottableItemDataAsset
{
	GENERATED_BODY()
	
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteObjectiveItem::StaticClass(); }
};