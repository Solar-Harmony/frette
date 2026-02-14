#include "Inventory/FretteInventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Frette/Frette.h"
#include "Net/UnrealNetwork.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

UFretteInventoryItem* UFretteInventoryComponent::GetItem(int32 Index)
{
	return Inventory.GetEntry(Index);
}

void UFretteInventoryComponent::SelectItem(const UFretteInventoryItem* Item) const
{
	check(Item);
	check(Item->GetOuter() == this);
	check(Item->Data);
	
	OnItemSelected.Broadcast(Item);
}

void UFretteInventoryComponent::AddItem_Implementation(UFretteInventoryItemDataAsset* Template)
{
	UFretteInventoryItem* Item = Template->CreateRuntimeItem(this);
	AddReplicatedSubObject(Item);
	Inventory.AddEntry(Item);
	IdToIndexMap.Add(Item->GetItemId(), Inventory.Num() - 1);
}

void UFretteInventoryComponent::EditItem_Implementation(UFretteInventoryItem* ModifiedItem)
{
	check(ModifiedItem);
	
	int32* Index = IdToIndexMap.Find(ModifiedItem->GetItemId());
	check(Index);
	Inventory.ChangeEntry(*Index, ModifiedItem);
}

void UFretteInventoryComponent::RemoveItem_Implementation(int32 Index)
{
	if (UFretteInventoryItem* Item = GetItem(Index))
	{
		RemoveReplicatedSubObject(Item);
		Inventory.RemoveEntry(Index);
		IdToIndexMap.Remove(Item->GetItemId());
	}
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		// AddReplicatedSubObject for default inventory items (none at the moment)
	}
}

void UFretteInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFretteInventoryComponent, Inventory, COND_OwnerOnly);
}