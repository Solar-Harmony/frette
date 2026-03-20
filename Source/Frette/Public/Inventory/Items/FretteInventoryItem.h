#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/UnrealNetwork.h"
#include "FretteInventoryItem.generated.h"

class AFretteBaseCharacter;
class UFretteInventoryComponent;

inline const FPrimaryAssetType GInventoryItemPrimaryAssetType("FretteInventoryItem");

UCLASS(Abstract, BlueprintType)
class FRETTE_API UFretteInventoryItem : public UObject
{
	GENERATED_BODY()
	
	friend class UFretteInventoryItemDataAsset;
	friend class UFretteInventoryComponent;

public:
	constexpr static int32 InvalidID = -1;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 Id = InvalidID;
	
	bool HasValidID() const { return Id != InvalidID; }
	
	UFretteInventoryItemDataAsset* GetUntypedData() const { return Data; }
	UFretteInventoryComponent* GetOwningInventory() const;
	AFretteBaseCharacter* GetOwningPlayer() const;
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, Data);
		DOREPLIFETIME(ThisClass, Id);
	}
	
private:
	void Use() { Use_Implementation(); }
	virtual void Use_Implementation() {}
	
	UPROPERTY(Replicated)
	UFretteInventoryItemDataAsset* Data = nullptr;	
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

#define FRETTE_ITEM_DATA_GETTER(DataClass) \
	template<typename T = DataClass> \
	FORCEINLINE T* GetData() const { return CastChecked<T>(GetUntypedData()); }
