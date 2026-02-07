#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Items/InventoryItemDataAsset.h"
#include "InventoryComponent.generated.h"

UINTERFACE()
class UInventoryComponent : public UInterface
{
	GENERATED_BODY()
};

class IInventoryComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UInventoryItem*)
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, UInventoryItem*)
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, UInventoryItem*)

	FOnItemAdded OnItemAdded;
	FOnItemRemoved OnItemRemoved;
	FOnItemSelected OnItemSelected;

	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) = 0;
};