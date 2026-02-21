#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "SlotsInventoryItemVM.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteGearItem.h"
#include "Player/FrettePlayerState.h"
#include "SlotsInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API USlotsInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<USlotsInventoryItemVM>> Items;

private:
	virtual void Bind() override
	{
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->SubToItemAdded(FOnItemAdded::FDelegate::CreateUObject(this, &USlotsInventoryVM::AddItem));
		Inventory->SubToItemRemoved(FOnItemRemoved::FDelegate::CreateUObject(this, &USlotsInventoryVM::RemoveItem));
	}

	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteGearItem>())
			return;

		auto* SubViewModel = NewObject<USlotsInventoryItemVM>(this);
		SubViewModel->SetModel(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}

	void RemoveItem(const UFretteInventoryItem* RemovedItem)
	{
		if (!RemovedItem->IsA<UFretteGearItem>())
			return;

		Items.RemoveAll([RemovedItem](const USlotsInventoryItemVM* ItemVM) {
			return ItemVM->GetPtr()->Id == RemovedItem->Id;
		});

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}

	UFUNCTION(BlueprintCallable)
	void OnItemSelectionChange(UObject* Item, bool bIsSelected) const
	{
		if (bIsSelected)
		{
			const auto* ItemVM = Cast<USlotsInventoryItemVM>(Item);
			PlayerCharacter->GetPlayerInventory()->SelectItem(ItemVM->GetPtr()->Id);
		}
	}
};