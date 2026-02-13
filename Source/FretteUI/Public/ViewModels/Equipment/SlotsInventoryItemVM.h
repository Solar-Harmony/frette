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
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetDisplayName() const { return Item->Data->DisplayName; }

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> Icon;

	void SetModel(const UFretteInventoryItem* InItem)
	{
		this->Item = InItem;
		this->Icon = Item->Data->Icon.LoadSynchronous(); // FIXME: Async load

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDisplayName);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Icon);
	}

	const UFretteInventoryItem* GetPtr() const { return Item; }

private:
	UPROPERTY()
	const UFretteInventoryItem* Item;
};