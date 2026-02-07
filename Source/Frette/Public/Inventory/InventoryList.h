#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryList.generated.h"

USTRUCT()
struct FInventoryListEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    UInventoryItem* Item = nullptr;
};

USTRUCT() 
struct FInventoryList : public FFastArraySerializer 
{
    GENERATED_BODY() 
    
    UPROPERTY() 
    TArray<FInventoryListEntry> Items; 
    
    UPROPERTY(NotReplicated) 
    IInventoryComponent* Owner = nullptr; 
    
    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams) 
    { 
        return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListEntry, FInventoryList>(Items, DeltaParams, *this); 
    } 
    
    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
    {
    	for (int32 Index : AddedIndices)
    	{
    		if (Owner)
    		{
    			UInventoryItem* Item = Items[Index].Item; 
    			Owner->OnItemAdded.Broadcast(Item);
    		}
    	}   
    }
//    void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
};

template<> struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};