#pragma once

#include "CoreMinimal.h"
#include "FretteClueInventoryVM.h"
#include "FretteViewModel.h"
#include "Game/Clues/FretteClueItem.h"
#include "Inventory/FretteInventoryComponent.h"
#include "FretteCluesInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteCluesInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	virtual void Bind() override
	{
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->SubToItemAdded(FOnItemAdded::FDelegate::CreateUObject(this, &UFretteCluesInventoryVM::AddItem));
	}
	
	void AddItem(const UFretteInventoryItem* NewItem)
	{
		const UFretteClueItem* ClueItem = Cast<UFretteClueItem>(NewItem);
		if (ClueItem == nullptr)
			return;

		UFretteClueInventoryVM* ItemVM = NewObject<UFretteClueInventoryVM>(this);
		ItemVM->SetFromModel(ClueItem);
		Items.Add(ItemVM);
		
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<UFretteClueInventoryVM*> Items;
};