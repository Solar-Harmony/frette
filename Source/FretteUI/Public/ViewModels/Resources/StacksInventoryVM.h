#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "StacksInventoryItemVM.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteStackableItem.h"
#include "Player/FrettePlayerState.h"
#include "StacksInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UStacksInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UStacksInventoryItemVM>> Items;
	
	virtual void Bind() override
	{
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->SubToItemAdded(FOnItemAdded::FDelegate::CreateUObject(this, &UStacksInventoryVM::AddItem));
		Inventory->SubToItemChanged(FOnItemChanged::FDelegate::CreateUObject(this, &UStacksInventoryVM::ChangeItem));
		RefreshAllItems();
	}

	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteStackableItem>())
			return;

		UStacksInventoryItemVM* SubViewModel = NewObject<UStacksInventoryItemVM>(this);
		SubViewModel->UpdateItem(Cast<UFretteStackableItem>(NewItem));
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
	
	void ChangeItem(const UFretteInventoryItem* Item)
	{
		const UFretteStackableItem* Stack = Cast<UFretteStackableItem>(Item);
		
		const auto* MaybeItemVM = Items.FindByPredicate([Item](const UStacksInventoryItemVM* VM) {
			return Item->Id == VM->ItemId;
		});
		
		if (MaybeItemVM == nullptr)
			return;	

		UStacksInventoryItemVM* ItemVM = *MaybeItemVM;
		ItemVM->UpdateItem(Stack);
	}
	
	void RefreshAllItems()
	{
		const UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		for (int32 Idx = 0; Idx < Inventory->GetNumItems(); ++Idx)
		{
			const UFretteInventoryItem* Item = Inventory->GetItemByIndexEditor(Idx);
			AddItem(Item);
		}
	}

	// TODO: Remove
};