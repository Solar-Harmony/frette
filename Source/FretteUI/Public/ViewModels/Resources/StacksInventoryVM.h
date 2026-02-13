#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "StacksInventoryItemVM.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteStackedItem.h"
#include "Player/FrettePlayerState.h"
#include "StacksInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UStacksInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UStacksInventoryItemVM>> Items;

	virtual void Bind(AFrettePlayerState* PlayerState) override
	{
		UFretteInventoryComponent* Inventory = PlayerState->GetPlayerInventory();
		Inventory->OnItemAdded.AddUObject(this, &UStacksInventoryVM::AddItem);
	}

	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteStackedItem>())
			return;

		UStacksInventoryItemVM* SubViewModel = NewObject<UStacksInventoryItemVM>(this);
		SubViewModel->SetName(NewItem);
		SubViewModel->SetIcon(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
};