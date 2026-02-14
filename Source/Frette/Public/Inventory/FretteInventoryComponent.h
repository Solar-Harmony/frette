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

	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	UFretteInventoryItem* GetItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category="Frette|Inventory")
	void SelectItem(const UFretteInventoryItem* Item) const;
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void AddItem(UFretteInventoryItemDataAsset* Template);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void EditItem(UFretteInventoryItem* ModifiedItem);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Frette|Inventory")
	void RemoveItem(int32 Index);

	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	TMap<FGuid, int32> IdToIndexMap; // used for O(1) lookups
	
	UPROPERTY(Replicated)
	FFretteInventoryList Inventory { this };
};