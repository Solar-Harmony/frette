#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/UnrealNetwork.h"
#include "FretteInventoryItem.generated.h"

class UFretteInventoryComponent;
class UFretteInventoryItemDataAsset;

inline const FPrimaryAssetType GInventoryItemPrimaryAssetType("FretteInventoryItem");

UCLASS(Abstract, BlueprintType, Blueprintable)
class FRETTE_API UFretteInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	constexpr static int32 InvalidID = -1;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 Id = InvalidID;

	UPROPERTY(BlueprintReadOnly, Replicated)
	UFretteInventoryItemDataAsset* Data = nullptr;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Use();
	
	virtual void Use_Implementation() {}
	
	UFretteInventoryComponent* GetOwningInventory() const;

	bool HasValidID() const { return Id != InvalidID; }

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, Data);
		DOREPLIFETIME(ThisClass, Id);
	}
};

UCLASS(Abstract, BlueprintType)
class UFretteInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UFretteInventoryItem* CreateRuntimeItem(UObject* OuterInventory)
	{
		UFretteInventoryItem* Item = NewObject<UFretteInventoryItem>(OuterInventory, GetRuntimeItemClass());
		Item->Data = this;
		InitializeItem(Item);
		return Item;
	}
	
	virtual const UClass* GetRuntimeItemClass() const PURE_VIRTUAL(GetRuntimeItemClass, return nullptr;);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GInventoryItemPrimaryAssetType, GetFName());
	}
	
protected:
	virtual void InitializeItem(UFretteInventoryItem* Item) {}
};