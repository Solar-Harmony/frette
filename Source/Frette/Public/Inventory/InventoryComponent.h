#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDataAsset.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UENUM()
enum class EInventoryQueryAttribute : uint8
{
	FirstFound,
	Durability
};

UENUM(BlueprintType)
enum class EComparisonPredicate : uint8
{
	Less UMETA(DisplayName="Less Than"),
	LessOrEqual UMETA(DisplayName="Less or Equal"),
	Equal UMETA(DisplayName="Equal"),
	GreaterOrEqual UMETA(DisplayName="Greater or Equal"),
	Greater UMETA(DisplayName="Greater Than"),
	NotEqual UMETA(DisplayName="Not Equal")
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UInventoryItemDataAsset* Data;

	UPROPERTY(BlueprintReadOnly)
	int Durability = 100;

	float GetAttributeValue(EInventoryQueryAttribute Attribute) const
	{
		switch (Attribute)
		{
			case EInventoryQueryAttribute::Durability:
				return static_cast<float>(Durability);
			default:
				return 0.0f;
		}
	}
};

USTRUCT(BlueprintType)
struct FInventoryQuery
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EInventoryQueryAttribute Attribute;

	UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "Attribute != EInventoryQueryAttribute::FirstFound"))
	EComparisonPredicate Predicate;

	UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "Attribute != EInventoryQueryAttribute::FirstFound"))
	float Value;
};

USTRUCT(BlueprintType)
struct FInventoryQueryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bFound;

	UPROPERTY(BlueprintReadOnly)
	FInventoryItem Result;
};

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent), Category="Inventory")
class FRETTE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FInventoryItem AddItem(UInventoryItemDataAsset* ItemType);

	TOptional<FInventoryItem> FindItemSafe(const UInventoryItemDataAsset* ItemType, const TFunctionRef<FInventoryItem*()>& Predicate);
	FInventoryItem* FindItem(const UInventoryItemDataAsset* ItemType, const TFunctionRef<bool(const FInventoryItem&)>& Predicate) const;

	UFUNCTION(BlueprintCallable)
	FInventoryQueryResult K2_FindItem(UInventoryItemDataAsset* ItemType, FInventoryQuery Query) const;

private:
	TMultiMap<FPrimaryAssetId, FInventoryItem> Items;
	static TArray<FInventoryItem> FoundItems;

	static bool CompareFloat(float A, float B, EComparisonPredicate Predicate)
	{
		switch (Predicate)
		{
			case EComparisonPredicate::Less:
				return A < B;
			case EComparisonPredicate::LessOrEqual:
				return A <= B;
			case EComparisonPredicate::Equal:
				return A == B;
			case EComparisonPredicate::GreaterOrEqual:
				return A >= B;
			case EComparisonPredicate::Greater:
				return A > B;
			case EComparisonPredicate::NotEqual:
				return A != B;
		}
		return false;
	}
};