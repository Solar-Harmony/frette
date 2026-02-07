#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryComponent.h"

#include "SlotsInventoryComponent.generated.h"

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent), Category="Inventory")
class FRETTE_API USlotsInventoryComponent : public UActorComponent, public IInventoryComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) override { return Items.Num(); }

	template <typename T>
	T* GetItem(int32 Index) { return Items[Index].GetMutablePtr<T>(); }

	template <typename T>
	T& GetItemChecked(int32 Index) { return Items[Index].Get<T>(); }

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Give Item"))
	int32 AddItem(UInventoryItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Remove Item"))
	void RemoveItem(int32 Index);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Swap Two Items"))
	void SwapItems(int32 FromIndex, int32 ToIndex);

	bool Generic_GetItem(int32 Index, const FStructProperty* OutItemProp, void* OutItemPtr);

	UFUNCTION(BlueprintCallable, CustomThunk, meta=(DisplayName="Get Item", CustomStructureParam="OutItem", ExpandBoolAsExecs="ReturnValue"))
	bool K2_GetItem(int32 Index, int32& OutItem);

	DECLARE_FUNCTION(execK2_GetItem);

private:
	UPROPERTY()
	TArray<FInstancedStruct> Items;
};