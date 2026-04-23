#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Inventory/Items/Impl/FretteClueItem.h"
#include "FretteClueInventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteClueInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	void SetFromModel(const UFretteClueItem* Item)
	{
		OwningInventory = Item->GetOwningInventory();
		UE_MVVM_SET_PROPERTY_VALUE(ItemID, Item->Id);
		UE_MVVM_SET_PROPERTY_VALUE(bIsPrimary, Item->bIsPrimary);
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, Item->GetUntypedData()->DisplayName);
		UE_MVVM_SET_PROPERTY_VALUE(Icon, Item->GetUntypedData()->Icon.LoadSynchronous()); // TODO: Use async load.
	}
	
protected:
	UPROPERTY()
	TObjectPtr<UFretteInventoryComponent> OwningInventory;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int32 ItemID;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText DisplayName;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	bool bIsPrimary;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> Icon;
	
	UFUNCTION(BlueprintCallable)
	void Use() const
	{
		OwningInventory->UseItem(ItemID);
	}
};