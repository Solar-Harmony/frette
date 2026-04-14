#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteStackableItem.h"
#include "StacksInventoryItemVM.generated.h"

class UFretteStackableItem;
UCLASS()
class FRETTEUI_API UStacksInventoryItemVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int ItemId;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int Quantity;
	
	void UpdateItem(const UFretteStackableItem* Item)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemId, Item->Id);
		UE_MVVM_SET_PROPERTY_VALUE(Name, Item->GetUntypedData()->DisplayName);
		UTexture2D* IconPtr = Item->GetUntypedData()->Icon.LoadSynchronous(); // TODO: Load async, or Common Lazy Image
		UE_MVVM_SET_PROPERTY_VALUE(Icon, IconPtr);
		UE_MVVM_SET_PROPERTY_VALUE(Quantity, Item->Quantity);
	}
};