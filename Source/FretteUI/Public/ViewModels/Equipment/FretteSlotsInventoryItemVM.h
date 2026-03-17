#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "FretteSlotsInventoryItemVM.generated.h"

class UFretteSlottableItem;
UCLASS()
class FRETTEUI_API UFretteSlotsInventoryItemVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int32 ItemID;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(BlueprintReadOnly, meta = (Categories = "Frette.Inventory.SlotType"))
	FGameplayTag SlotType;
	
	UPROPERTY()
	TObjectPtr<UFretteInventoryComponent> OwningInventory;

	void SetFromModel(UFretteSlottableItem* Item)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemID, Item->Id);
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, Item->GetData()->DisplayName);
		UE_MVVM_SET_PROPERTY_VALUE(Icon, Item->GetData()->Icon.LoadSynchronous()); // TODO: Use async load.
		OwningInventory = Item->GetOwningInventory();
		SlotType = Item->GetData()->ItemSlotTag;
		Ptr = Item;
	}
	
	// TODO: Temp
	UPROPERTY()
	TObjectPtr<UFretteInventoryItem> Ptr = nullptr;
};