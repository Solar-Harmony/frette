// 

#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "Components/ActorComponent.h"
#include "InventoryActorComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()
	
	FInventoryItem()
		: Data(nullptr), InstanceId(FGuid::NewGuid())
	{ }
	FInventoryItem(UItemDataAsset* Data)
		: Data(Data), InstanceId(FGuid::NewGuid())
	{ }
	
	UPROPERTY()
	UItemDataAsset* Data;
	
	UPROPERTY(BlueprintReadOnly)
	FGuid InstanceId;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Category="Inventory")
class FRETTE_API UInventoryActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryActorComponent();

protected:
	// Will init the map of names to data assets
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	// Adding stuff
	UFUNCTION(BlueprintCallable)
	FGuid AddItem(FName ItemId);
	
	UFUNCTION(BlueprintCallable)
	TArray<FGuid> AddItems(FName ItemId, int32 Quantity);
	
	// Removing stuff
	UFUNCTION(BlueprintCallable)
	bool RemoveItem(FGuid Item);
	
	UFUNCTION(BlueprintCallable)
	int32 RemoveItems(const TArray<FGuid>& Items);
	
	UFUNCTION(BlueprintCallable)
	int32 RemoveItemsOfId(FName ItemId);
	
	// Getting stuff
	UFUNCTION(BlueprintCallable)
	FGuid GetFirstItem(FName ItemId) const;
	
	UFUNCTION(BlueprintCallable)
	TArray<FGuid> GetItems(FName ItemId) const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetTotalNumItems() const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetNumItems(FName ItemId) const;
	
	// Predicates
	UFUNCTION(BlueprintCallable)
	bool IsFull() const;
	
	UFUNCTION(BlueprintCallable)
	bool HasItem(FName ItemId) const;
	
	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryReady);
	UPROPERTY(BlueprintAssignable)
	FOnInventoryReady OnInventoryReady;
	
	// Visible state in blueprints for debugging
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FInventoryItem> Items;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TMap<FName, UItemDataAsset*> ItemIdLookup;
	
	// Modifiable state in blueprints
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	int32 MaxNumItems = 100;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	int32 MaxArea = 100;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	bool bUseArea = false;
	
private:
	void OnItemDataAssetsLoaded();
	
	const FPrimaryAssetType ITEM_TYPE = FPrimaryAssetType("Item");
	FGuid BadGuid = FGuid::NewGuid();
};
