#pragma once

#include "CoreMinimal.h"
#include "FretteSlotsInventoryItemVM.h"
#include "FretteSlotsInventorySlotVM.h"
#include "FretteViewModel.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "FretteSlotsInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteSlotsInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()
	
	friend class UFretteInventoryWidget;

protected:
	// fixed list of slots, populated by the widget
	UPROPERTY()
	TArray<TObjectPtr<UFretteSlotsInventorySlotVM>> Slots;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UFretteSlotsInventoryItemVM>> Items;
	
	UFUNCTION(BlueprintCallable)
	void SelectSlot(UFretteSlotsInventorySlotVM* SlotVM)
	{
		if (!SlotVM->ContainsItem())
			return;

		UFretteSlotsInventoryItemVM* ItemVM = SlotVM->ItemVM;
		const int32 NewSlotIdx = FindFirstFreeSlot(ItemVM, SlotVM->IsCompatibleWithAnything());
		if (NewSlotIdx == INDEX_NONE)
		{
			// couldn't find a slot to move the item in
			return;
		}
		
		// swap the 2 items
		UFretteSlotsInventorySlotVM* NewSlotVM = Slots[NewSlotIdx];
		SlotVM->SetItemVM(NewSlotVM->ItemVM);
		NewSlotVM->SetItemVM(ItemVM);

		const UFretteEquipmentComponent* EquipmentComponent = PlayerCharacter->GetEquipmentComponent();
		UFretteSlottableItem* Item = Cast<UFretteSlottableItem>(ItemVM->Ptr);
		if (NewSlotVM->CompatibleSlotType == FGameplayTag::EmptyTag)
		{
			EquipmentComponent->UnequipItem(Item);
		}
		else
		{
			EquipmentComponent->EquipItem(Item);
		}		
	}
	
private:
	virtual void Bind() override
	{
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->SubToItemAdded(FOnItemAdded::FDelegate::CreateUObject(this, &UFretteSlotsInventoryVM::AddItem));
		Inventory->SubToItemRemoved(FOnItemRemoved::FDelegate::CreateUObject(this, &UFretteSlotsInventoryVM::RemoveItem));
	}
	
	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteSlottableItem>())
			return;

		UFretteSlotsInventoryItemVM* ItemVM = NewObject<UFretteSlotsInventoryItemVM>(this);
		ItemVM->SetFromModel(NewItem);
		Items.Add(ItemVM);
		
		const int32 FreeSlotIdx = FindFirstFreeSlot(ItemVM, false);
		if (FreeSlotIdx == INDEX_NONE)
		{
			// FIXME: We need to refuse adding item ON THE SERVER if there are no more slots!!!
			// This is why the slot system should be on the server but i'm a Dumbass and did it 
			// in the viewmodel and we don't have time do think of better. For now just notify
			ensure(false);
			return;
		}
		
		Slots[FreeSlotIdx]->SetItemVM(ItemVM);
		UpdateItemSlot(ItemVM, FreeSlotIdx);
		
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}

	void RemoveItem(const UFretteInventoryItem* RemovedItem)
	{
		const auto* SlotItem = Cast<UFretteSlottableItem>(RemovedItem);
		check(SlotItem);

		Slots[SlotItem->SlotID]->SetItemVM(nullptr);
		Items.RemoveAll([RemovedItem](const UFretteSlotsInventoryItemVM* ItemVM) {
			return ItemVM->ItemID == RemovedItem->Id;
		});
		
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
	
	int32 FindFirstFreeSlot(const UFretteSlotsInventoryItemVM* ItemVM, bool bNeedExplicitCompatibility)
	{
		for (int Idx = 0; Idx < Slots.Num(); ++Idx)
		{
			const UFretteSlotsInventorySlotVM* Slot = Slots[Idx];
			check(Slot != nullptr);
			
			if (Slot->ContainsItem())
				continue;
			
			const bool bCompatible = bNeedExplicitCompatibility
				? Slot->CompatibleSlotType == ItemVM->SlotType
				: Slot->IsCompatibleWithAnything();
			
			if (bCompatible) 
				return Idx;
		}
		
		return INDEX_NONE;
	}
	
	// FIXME: We are client authoritative. This is bad and a hack for the demo to work
	// Have to rework the slot system into server
	static void UpdateItemSlot(const UFretteSlotsInventoryItemVM* ItemVM, int32 SlotID)
	{
		UFretteSlottableItem* GearItem = Cast<UFretteSlottableItem>(ItemVM->Ptr);
		GearItem->SlotID = SlotID;

		UFretteInventoryComponent* Inventory = ItemVM->OwningInventory;
		Inventory->ChangeItem(GearItem);
	}
};