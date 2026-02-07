#pragma once

#include "CoreMinimal.h"
#include "InventoryList.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryComponent.h"

#include "SlotsInventoryComponent.generated.h"

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent), Category="Inventory")
class FRETTE_API USlotsInventoryComponent : public UActorComponent, public IInventoryComponent
{
	GENERATED_BODY()

public:
	USlotsInventoryComponent();
	
	UFUNCTION(BlueprintCallable)
	virtual int32 GetItemCount(UInventoryItemDataAsset* ItemType) override;

	UFUNCTION(BlueprintPure, Category="Inventory")
	UInventoryItem* GetItem(int32 Index);

	template <typename T>
	T* GetItem(int32 Index) { return Cast<T>(GetItem(Index)); }

	template <typename T>
	const T* GetItem(int32 Index) const { return GetItem<T>(Index); }

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Give Item"))
	int32 AddItem(UInventoryItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Remove Item"))
	void RemoveItem(int32 Index);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Swap Two Items"))
	void SwapItems(int32 FromIndex, int32 ToIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	
private:
	UPROPERTY(Replicated)
	FInventoryList Inventory;
};
