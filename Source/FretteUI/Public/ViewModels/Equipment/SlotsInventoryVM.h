#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "SlotsInventoryItemVM.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteSlottedItem.h"
#include "Player/FrettePlayerState.h"
#include "SlotsInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API USlotsInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<USlotsInventoryItemVM>> Items;

	virtual void Bind(AFrettePlayerState* PlayerState) override
	{
		UFretteInventoryComponent* Inventory = PlayerState->GetPlayerInventory();
		Inventory->OnItemAdded.AddUObject(this, &USlotsInventoryVM::AddItem);
	}

	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteSlottedItem>())
			return;

		USlotsInventoryItemVM* SubViewModel = NewObject<USlotsInventoryItemVM>(this);
		SubViewModel->SetName(NewItem);
		SubViewModel->SetIcon(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
};