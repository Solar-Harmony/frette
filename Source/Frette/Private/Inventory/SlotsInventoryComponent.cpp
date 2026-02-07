#include "Inventory/SlotsInventoryComponent.h"
#include "UObject/Stack.h"
#include "UObject/UnrealType.h"

int32 USlotsInventoryComponent::AddItem(UInventoryItemDataAsset* ItemData)
{
	check(ItemData);
	const UScriptStruct* RuntimeDataType = ItemData->GetRuntimeDataType();
	checkf(RuntimeDataType, TEXT("Item data asset %s has an invalid runtime data type."), *ItemData->GetName());
	checkf(RuntimeDataType->IsChildOf(FInventoryItem::StaticStruct()), TEXT("Item data asset %s has a runtime data type that is not a child of FInventoryItem."), *ItemData->GetName());

	FInstancedStruct NewItem;
	NewItem.InitializeAs(RuntimeDataType);

	FInventoryItem* ItemPtr = NewItem.GetMutablePtr<FInventoryItem>();
	check(ItemPtr);
	ItemPtr->Data = ItemData;

	const int32 Index = Items.Add(NewItem);
	OnItemAdded.Broadcast(Items[Index]);
	return Index;
}

void USlotsInventoryComponent::RemoveItem(int32 Index)
{
	ensureAlwaysMsgf(Items.IsValidIndex(Index), TEXT("[%s|Slots Inventory] Tried to remove item at invalid index %d."), *GetOwner()->GetName(), Index);
	Items.RemoveAt(Index);
}

void USlotsInventoryComponent::SwapItems(int32 FromIndex, int32 ToIndex)
{
	ensureAlwaysMsgf(Items.IsValidIndex(FromIndex), TEXT("[%s|Slots Inventory] Tried to swap item from invalid index %d."), *GetOwner()->GetName(), FromIndex);
	ensureAlwaysMsgf(Items.IsValidIndex(ToIndex), TEXT("[%s|Slots Inventory] Tried to swap item to invalid index %d."), *GetOwner()->GetName(), ToIndex);
	Items.Swap(FromIndex, ToIndex);
}

bool USlotsInventoryComponent::Generic_GetItem(int32 Index, const FStructProperty* OutItemProp, void* OutItemPtr)
{
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	const FInstancedStruct& Item = Items[Index];
	if (!Item.IsValid())
	{
		return false;
	}

	const UScriptStruct* ScriptStruct = Item.GetScriptStruct();
	const UScriptStruct* OutStruct = OutItemProp->Struct;

	if (!ScriptStruct || !OutStruct)
	{
		return false;
	}

	if (ScriptStruct->IsChildOf(OutStruct))
	{
		OutStruct->CopyScriptStruct(OutItemPtr, Item.GetMemory());
		return true;
	}

	return false;
}

bool USlotsInventoryComponent::K2_GetItem(int32 Index, int32& OutItem)
{
	checkNoEntry();
	return false;
}

DEFINE_FUNCTION(USlotsInventoryComponent::execK2_GetItem)
{
	P_GET_PROPERTY(FIntProperty, Index);

	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* OutItemPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* OutItemProp = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH;

	bool bSuccess = false;
	if (OutItemPtr && OutItemProp)
	{
		P_NATIVE_BEGIN;
			bSuccess = P_THIS->Generic_GetItem(Index, OutItemProp, OutItemPtr);
		P_NATIVE_END;
	}
	*(bool*)RESULT_PARAM = bSuccess;
}