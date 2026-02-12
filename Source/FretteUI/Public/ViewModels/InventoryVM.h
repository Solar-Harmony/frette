#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "InventoryItemVM.h"
#include "Inventory/InventoryComponent.h"
#include "Player/FrettePlayerState.h"
#include "InventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UInventoryItemVM>> Items;

	virtual void Bind(AFrettePlayerState* PlayerState) override
	{
		UFretteInventoryComponent* Inventory = PlayerState->GetPlayerInventory();
		Inventory->OnItemAdded.AddUObject(this, &UInventoryVM::AddItem);
	}

	void AddItem(const UInventoryItem* NewItem)
	{
		UInventoryItemVM* SubViewModel = NewObject<UInventoryItemVM>(this);
		SubViewModel->SetName(NewItem);
		SubViewModel->SetIcon(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
};