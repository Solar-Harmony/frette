#pragma once

#include "CoreMinimal.h"
#include "FretteInventoryList.h"
#include "Components/ActorComponent.h"

#include "FretteInventoryComponent.generated.h"

class UFretteStackableItemDataAsset;
class UFretteInventoryItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const UFretteInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChanged, const UFretteInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const UFretteInventoryItem*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, const UFretteInventoryItem*)

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteInventoryComponent();

	FOnItemAdded OnItemAdded;
	FOnItemChanged OnItemChanged;
	FOnItemRemoved OnItemRemoved;
	FOnItemSelected OnItemSelected;

	UFUNCTION(BlueprintPure, Category="Frette|Inventory", meta = (DisplayName="Number of Items"))
	int32 GetNumItems() const { return Inventory.Num(); }

	template <typename T>
	const T* GetItem(int32 Id) const { return Cast<T>(GetItem(Id)); }

	UFUNCTION(BlueprintPure, Category="Frette|Inventory")
	UFretteInventoryItem* GetItem(int32 Id) const;

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