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
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFretteSlotsInventoryVM> ParentVM = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (Categories = "Frette.Inventory.SlotType"))
	FGameplayTag CompatibleSlotType;
	
	UFUNCTION(BlueprintPure, FieldNotify)
	bool ContainsItem() const { return ItemVM != nullptr; }
	
	// Looks like MVVM plugin doesn't support nullable bindings (it spams error logs when ItemVM is nullptr)
	// But it should be perfectly valid for a slot to have no item.
	// So I return the CDO when there's no item to please the framework.
	// I feel this is actually pretty clean since the CDO serves its purpose of - uh, default object - well.
	// But there might be a more idiomatic way to do this.
	UFUNCTION(BlueprintPure, FieldNotify)
	const UFretteSlotsInventoryItemVM* GetItemVM() const
	{
		return ItemVM == nullptr ? GetDefault<UFretteSlotsInventoryItemVM>() : ItemVM;
	}
	
	UFretteSlotsInventoryItemVM* GetItemVM()
	{
		return ItemVM == nullptr ? GetMutableDefault<UFretteSlotsInventoryItemVM>() : ItemVM;
	}
	
	void SetItemVM(UFretteSlotsInventoryItemVM* NewItemVM)
	{
		ItemVM = NewItemVM;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetItemVM);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ContainsItem);
	}
	
	bool IsCompatibleWithAnything() const { return CompatibleSlotType == FGameplayTag::EmptyTag; }
	
	void SetCompatibleTag(FGameplayTag Tag)
	{
		CompatibleSlotType = Tag;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCompatibleSlotName);
	}
	
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetCompatibleSlotName() const { return FText::FromName(CompatibleSlotType.GetTagLeafName()); }

	UPROPERTY()
	UFretteSlotsInventoryItemVM* ItemVM = nullptr;
};