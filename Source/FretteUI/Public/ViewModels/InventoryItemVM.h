#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "FakeInventoryComponent.h"
#include "MVVMViewModelBase.h"
#include "InventoryItemVM.generated.h"

UCLASS()
class FRETTEUI_API UInventoryItemVM : public UFretteViewModel
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText Name;
	
	void SetName(const FInventoryItem& Item)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Name, FText::FromName(Item.ItemName));
	}
};