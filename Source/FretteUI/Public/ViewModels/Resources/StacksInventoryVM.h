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
	}

	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteStackableItem>())
			return;

		UStacksInventoryItemVM* SubViewModel = NewObject<UStacksInventoryItemVM>(this);
		SubViewModel->SetName(NewItem);
		SubViewModel->SetIcon(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}

	// TODO: Remove
};