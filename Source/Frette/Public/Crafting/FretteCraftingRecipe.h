#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/FretteStackableItem.h"
#include "Misc/DataValidation.h"
#include "FretteCraftingRecipe.generated.h"

UCLASS(Abstract, BlueprintType)
class FRETTE_API UFretteAbstractRecipe : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TMap<TObjectPtr<UFretteStackableItemDataAsset>, int32> RequiredItems;
};

UCLASS(BlueprintType)
class FRETTE_API UFretteItemCraftingRecipe : public UFretteAbstractRecipe
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteInventoryItemDataAsset> ItemToCraft;
	
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		if (!IsValid(ItemToCraft))
		{
			Context.AddError(INVTEXT("No item to craft specified in the recipe."));
			return EDataValidationResult::Invalid;
		}
		
		return Super::IsDataValid(Context);
	}
};

UCLASS(BlueprintType)
class FRETTE_API UFrettePlaceableCraftingRecipe : public UFretteAbstractRecipe
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> PlacementGhostMesh;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawn;
	
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		if (PlacementGhostMesh.IsNull())
		{
			Context.AddError(INVTEXT("No placement ghost mesh specified in the recipe."));
			return EDataValidationResult::Invalid;
		}
		
		if (!IsValid(ActorToSpawn))
		{
			Context.AddError(INVTEXT("No actor to spawn specified in the recipe."));
			return EDataValidationResult::Invalid;
		}
		
		return Super::IsDataValid(Context);
	}
};