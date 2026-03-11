#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "FretteSlotsInventorySlotVM.generated.h"

class UFretteSlotsInventoryVM;
class UFretteSlotsInventoryItemVM;

UCLASS()
class FRETTEUI_API UFretteSlotsInventorySlotVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter)
	TObjectPtr<UFretteSlotsInventoryItemVM> ItemVM = nullptr;

	void SetItemVM(UFretteSlotsInventoryItemVM* NewItemVM)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemVM, NewItemVM);
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UFretteSlotsInventoryVM> ParentVM = nullptr;
};