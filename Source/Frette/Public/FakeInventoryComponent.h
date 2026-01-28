#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FakeInventoryComponent.generated.h"

// Used as a stub for UI prototyping while William works on the real thing. Ignore.

class USpecialEffectConfig;

UCLASS(BlueprintType, Blueprintable)
class UInventoryItemTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName ItemName;
	
	UPROPERTY(EditDefaultsOnly)
	FColor Color;
	
	UPROPERTY(EditDefaultsOnly)
	int MaxDurability = 50;
	
	UPROPERTY()
	bool bHasSpecialEffect = false;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bHasSpecialEffect"))
	TSoftObjectPtr<USpecialEffectConfig> SpecialEffectConfig;
};

UCLASS(BlueprintType, Blueprintable)
class USpecialEffectConfig : public UDataAsset
{
	GENERATED_BODY()

	
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()
	
	FInventoryItem() = default;

	explicit FInventoryItem(const UInventoryItemTemplate* ItemTemplate)
		: ItemName(ItemTemplate->ItemName)
		, Color(ItemTemplate->Color)
		, Durability(ItemTemplate->MaxDurability)
	{
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName ItemName;
	
	UPROPERTY()
	FColor Color;
	
	UPROPERTY()
	int Durability;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemAdded, const FInventoryItem&, NewItem);

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFakeInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemAdded OnItemAdded;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(UInventoryItemTemplate* Item)
	{
		Items.Add(FInventoryItem(Item));
		OnItemAdded.Broadcast(Items.Last());
	}
	
	UFUNCTION(BlueprintCallable)
	const TArray<FInventoryItem>& GetItems() const
	{
		return Items;
	}
	
private:
	UPROPERTY(Transient)
	TArray<FInventoryItem> Items;
};
