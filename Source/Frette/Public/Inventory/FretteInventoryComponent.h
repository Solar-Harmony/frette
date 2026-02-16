#pragma once

#include "CoreMinimal.h"
#include "FretteInventoryList.h"
#include "Components/ActorComponent.h"

#include "FretteInventoryComponent.generated.h"

class UFretteStackableItemDataAsset;
class UFretteInventoryItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const UFretteInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const UFretteInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, const UFretteInventoryItem*)

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteInventoryComponent();

	FOnItemAdded OnItemAdded;
	FOnItemRemoved OnItemRemoved;
	FOnItemSelected OnItemSelected;

	template <typename T>
	const T* GetItem(int32 Index) const { return GetItem<T>(Index); }

	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	int32 GetNumItems() const { return Inventory.Num(); }

	// Warning: the index is not stable and may change when items are added/removed. 
	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	UFretteInventoryItem* GetItemByIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category="Frette|Inventory")
	void SelectItem(const UFretteInventoryItem* Item) const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void AddItem(UFretteInventoryItemDataAsset* ItemData);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void ChangeItem(UFretteInventoryItem* ItemToChange);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void RemoveItem(UFretteInventoryItem* ItemToRemove);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	FFretteInventoryList Inventory{ this };
};