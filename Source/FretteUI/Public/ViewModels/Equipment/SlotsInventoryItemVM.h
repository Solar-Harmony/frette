#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Inventory/FretteInventoryComponent.h"
#include "SlotsInventoryItemVM.generated.h"

UCLASS()
class FRETTEUI_API USlotsInventoryItemVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> Icon;

	void SetName(const UFretteInventoryItem* Item)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Name, Item->Data->DisplayName);
	}

	void SetIcon(const UFretteInventoryItem* Item)
	{
		// we might want async loading, or preloading
		// also check out Common Lazy Image?
		UTexture2D* IconPtr = Item->Data->Icon.LoadSynchronous();
		UE_MVVM_SET_PROPERTY_VALUE(Icon, IconPtr);
	}
};