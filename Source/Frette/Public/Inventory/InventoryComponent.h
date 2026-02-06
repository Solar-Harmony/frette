#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDataAsset.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UInventoryItemDataAsset* Data;
};

UINTERFACE()
class UInventoryComponent : public UInterface
{
	GENERATED_BODY()
};

class IInventoryComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const FInventoryItem&)
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const FInventoryItem&)
	
	FOnItemAdded OnItemAdded;
	FOnItemRemoved OnItemRemoved;
	
	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) = 0;
};
