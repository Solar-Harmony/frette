#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Inventory/FretteInventoryComponent.h"
#include "FretteSlotsInventoryItemVM.generated.h"

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
	
	UPROPERTY()
	TObjectPtr<UFretteInventoryComponent> OwningInventory;

	void SetFromModel(const UFretteInventoryItem* Item)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemID, Item->Id);
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, Item->Data->DisplayName);
		UE_MVVM_SET_PROPERTY_VALUE(Icon, Item->Data->Icon.LoadSynchronous()); // TODO: Use async load.
		OwningInventory = Item->GetOwningInventory();
		Ptr = const_cast<UFretteInventoryItem*>(Item);
	}
	
	// TODO: Temp
	UPROPERTY()
	TObjectPtr<UFretteInventoryItem> Ptr = nullptr;
};