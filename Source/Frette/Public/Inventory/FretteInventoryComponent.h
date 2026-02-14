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
	T* GetItem(int32 Index) { return Cast<T>(GetItem(Index)); }

	template <typename T>
	const T* GetItem(int32 Index) const { return GetItem<T>(Index); }

	UFUNCTION(BlueprintPure, Category="Frette Inventory")
	UFretteInventoryItem* GetItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category="Frette Inventory")
	bool SelectItem(int32 Index);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void AddItem(UFretteInventoryItemDataAsset* Template);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(int32 Index);

	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

private:
	UPROPERTY()
	TMap<int32, UFretteInventoryItem*> LookupCache;
	
	UPROPERTY(Replicated)
	FFretteInventoryList Inventory { this };
};