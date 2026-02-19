#pragma once

#include "CoreMinimal.h"
#include "FretteInventoryList.h"
#include "Components/ActorComponent.h"

#include "FretteInventoryComponent.generated.h"

class UFretteStackableItemDataAsset;
class UFretteInventoryItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const UFretteInventoryItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChanged, const UFretteInventoryItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const UFretteInventoryItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, const UFretteInventoryItem*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FK2_OnItemAdded, const UFretteInventoryItem*, AddedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FK2_OnItemChanged, const UFretteInventoryItem*, ChangedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FK2_OnItemRemoved, const UFretteInventoryItem*, RemovedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FK2_OnItemSelected, const UFretteInventoryItem*, SelectedItem);

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	// so the backing array wrapper can fire delegates
	friend struct FFretteInventoryList;

public:
	UFretteInventoryComponent();

	FORCEINLINE void SubToItemAdded(const FOnItemAdded::FDelegate& Delegate)
	{
		OnItemAdded.Add(Delegate);
	}

	FORCEINLINE void SubToItemChanged(const FOnItemChanged::FDelegate& Delegate)
	{
		OnItemChanged.Add(Delegate);
	}

	FORCEINLINE void SubToItemRemoved(const FOnItemRemoved::FDelegate& Delegate)
	{
		OnItemRemoved.Add(Delegate);
	}

	FORCEINLINE void SubToItemSelected(const FOnItemSelected::FDelegate& Delegate)
	{
		OnItemSelected.Add(Delegate);
	}

	UFUNCTION(BlueprintPure, Category="Frette|Inventory", meta = (DisplayName="Number of Items"))
	int32 GetNumItems() const { return Inventory.Num(); }

	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	bool HasItem(int32 Id) const { return Inventory.HasEntry(Id); }

	template <typename T>
	const T* GetItem(int32 Id) const { return Cast<T>(GetItem(Id)); }

	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	UFretteInventoryItem* GetItem(int32 Id) const { return Inventory.GetItemById(Id); }

	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	bool IsItemValid(const UFretteInventoryItem* Item) const { return Inventory.IsValidItem(Item); }

	UFUNCTION(BlueprintCallable, Category="Frette|Inventory")
	void SelectItem(int32 ItemId) const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void AddItem(UFretteInventoryItemDataAsset* ItemData);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void ChangeItem(UFretteInventoryItem* ItemToChange);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void RemoveItem(int32 ItemId);

	virtual void ReadyForReplication() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#if WITH_EDITOR
	void DumpInventory() const { Inventory.DumpInventory(); }
#endif

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UFretteInventoryItemDataAsset>> StartingItems;

private:
	FOnItemAdded OnItemAdded;
	FOnItemChanged OnItemChanged;
	FOnItemRemoved OnItemRemoved;
	FOnItemSelected OnItemSelected;

	UPROPERTY(BlueprintAssignable, Category="Frette|Inventory", meta=(DisplayName="On Item Added"))
	FK2_OnItemAdded K2_OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category="Frette|Inventory", meta=(DisplayName="On Item Changed"))
	FK2_OnItemChanged K2_OnItemChanged;

	UPROPERTY(BlueprintAssignable, Category="Frette|Inventory", meta=(DisplayName="On Item Pending Remove"))
	FK2_OnItemRemoved K2_OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category="Frette|Inventory", meta=(DisplayName="On Item Selected"))
	FK2_OnItemSelected K2_OnItemSelected;

	UPROPERTY(Replicated)
	FFretteInventoryList Inventory{ this };
};