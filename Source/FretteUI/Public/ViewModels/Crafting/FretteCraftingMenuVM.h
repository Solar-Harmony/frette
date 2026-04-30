#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Inventory/Items/Impl/FretteClueItem.h"
#include "FretteCraftingMenuVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteCraftingMenuItemVM : public UFretteViewModel
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int RecipeIdx;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	bool bAvailableToCraft;
	
	// void UpdateItem(const UFretteCraftingRecipe_Item* Item, int Idx, bool bAvailable)
	// {
	// 	UE_MVVM_SET_PROPERTY_VALUE(RecipeIdx, Idx);
	// 	UE_MVVM_SET_PROPERTY_VALUE(Name, Item->ItemToCraft->DisplayName);
	// 	UTexture2D* IconPtr = Item->ItemToCraft->Icon.LoadSynchronous(); // TODO: Load async, or Common Lazy Image
	// 	UE_MVVM_SET_PROPERTY_VALUE(Icon, IconPtr);
	// 	UE_MVVM_SET_PROPERTY_VALUE(bAvailableToCraft, bAvailable);
	// }
	
	UFUNCTION(BlueprintCallable)
	void Craft() const
	{
		// PlayerCharacter->CraftingComponent->Craft(RecipeIdx);
		// PlayerCharacter->CraftingComponent->UpdateAvailableRecipes();
	}
};

UCLASS()
class FRETTEUI_API UFretteCraftingMenuVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	virtual void Bind() override
	{
		// PlayerCharacter->CraftingComponent->OnAvailableRecipesChanged.AddUObject(this, &UFretteCraftingMenuVM::UpdateRecipesList);
		// UpdateRecipesList();
	}
	
	void UpdateRecipesList()
	{
		// const TArray<UFretteCraftingRecipe_Item*>& Recipes = PlayerCharacter->CraftingComponent->GetAvailableRecipes();
		// for (int Idx = 0; Idx < Recipes.Num(); ++Idx)
		// {
		// 	const auto& Recipe = Recipes[Idx];
		// 	auto* ItemVM = CreateFretteViewModel<UFretteCraftingMenuItemVM>(this, PlayerCharacter);
		// 	const bool bAvailableRecipe = PlayerCharacter->CraftingComponent->GetRecipesAvailability()[Idx];
		// 	ItemVM->UpdateItem(Recipe, Idx, bAvailableRecipe);
		// 	Items.Add(ItemVM);
		// }
		//
		// UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UFretteCraftingMenuItemVM>> Items;
};
