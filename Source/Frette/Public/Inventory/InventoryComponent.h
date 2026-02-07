#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "InventoryItemDataAsset.h"
#include "StructView.h"
#include "Components/ActorComponent.h"
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
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const FInstancedStruct&)
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const FInstancedStruct&)
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, const FInstancedStruct&)

	FOnItemAdded OnItemAdded;
	FOnItemRemoved OnItemRemoved;
	FOnItemSelected OnItemSelected;

	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) = 0;
};