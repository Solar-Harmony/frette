#pragma once

#include "CoreMinimal.h"
#include "FretteSlotsInventoryItemVM.h"
#include "FretteSlotsInventorySlotVM.h"
#include "FretteViewModel.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "Widgets/FretteInventorySlotWidget.h"
#include "FretteSlotsInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteSlotsInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<UFretteSlotsInventorySlotVM> HeldItem = nullptr;
	
	UPROPERTY(FieldNotify)
	TMap<int32, TObjectPtr<UFretteSlotsInventorySlotVM>> SlotIDToItemMap;
	
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UFretteSlotsInventoryItemVM>> Items;
	
private:
	virtual void Bind() override
	{
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->SubToItemAdded(FOnItemAdded::FDelegate::CreateUObject(this, &UFretteSlotsInventoryVM::AddItem));
		Inventory->SubToItemRemoved(FOnItemRemoved::FDelegate::CreateUObject(this, &UFretteSlotsInventoryVM::RemoveItem));
	}

	static void UpdateItemSlot(const UFretteSlotsInventoryItemVM* ItemVM, int32 SlotID)
	{
		const UFretteSlottableItem* GearItem = Cast<UFretteSlottableItem>(ItemVM->Ptr);
		// todo: temp const cast lol
		UFretteSlottableItem* MutableGearItem = const_cast<UFretteSlottableItem*>(GearItem);
		MutableGearItem->SlotID = SlotID;

		UFretteInventoryComponent* Inventory = ItemVM->OwningInventory;
		Inventory->ChangeItem(MutableGearItem);
	}
	
	void AddItem(const UFretteInventoryItem* NewItem)
	{
		if (!NewItem->IsA<UFretteSlottableItem>())
			return;

		auto* SubViewModel = NewObject<UFretteSlotsInventoryItemVM>(this);
		SubViewModel->SetFromModel(NewItem);
		Items.Add(SubViewModel);
		
		const int32 FreeSlot = FindFirstFreeSlot();
		SlotIDToItemMap[FreeSlot]->SetItemVM(SubViewModel);
		UpdateItemSlot(SubViewModel, FreeSlot);
		
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SlotIDToItemMap);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}

	void RemoveItem(const UFretteInventoryItem* RemovedItem)
	{
		const auto* SlotItem = Cast<UFretteSlottableItem>(RemovedItem);
		check(SlotItem);

		SlotIDToItemMap[SlotItem->SlotID]->SetItemVM(nullptr);
		Items.RemoveAll([RemovedItem](const UFretteSlotsInventoryItemVM* ItemVM) {
			return ItemVM->ItemID == RemovedItem->Id;
		});
		
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SlotIDToItemMap);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
	
	int32 FindFirstFreeSlot()
	{
		for (const auto& Pair : SlotIDToItemMap)
		{
			check(Pair.Value != nullptr);
			
			if (Pair.Value->ItemVM == nullptr)
			{
				return Pair.Key;
			}
		}
		
		checkNoEntry();
		return -1;
	}

	UFUNCTION(BlueprintCallable)
	void OnItemSelectionChange(UObject* Item, bool bIsSelected) const
	{
		if (bIsSelected)
		{
			const auto* ItemVM = Cast<UFretteSlotsInventoryItemVM>(Item);
			PlayerCharacter->GetPlayerInventory()->SelectItem(ItemVM->ItemID);
		}
	}
};