#pragma once

#include "CoreMinimal.h"
#include "InventoryList.h"
#include "Components/ActorComponent.h"

#include "InventoryComponent.generated.h"

class UInventoryStackDataAsset;
class UInventoryItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const UInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const UInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, const UInventoryItem*)

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
	T* GetItem(int32 Index) { return Cast<T>(GetItem(Index)); }

	template <typename T>
	const T* GetItem(int32 Index) const { return GetItem<T>(Index); }

	UFUNCTION(BlueprintPure, Category="Frette Inventory")
	UInventoryItem* GetItem(int32 Index) const;

	UFUNCTION(Server, Reliable, BlueprintCallable, meta=(DisplayName="Give Item"))
	void AddItem(UInventoryItemDataAsset* Template);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Remove Item"))
	void RemoveItem(int32 Index);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Swap Two Items"))
	void SwapItems(int32 FromIndex, int32 ToIndex);

	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

private:
	UPROPERTY(Replicated)
	FInventoryList Inventory{ this };
};