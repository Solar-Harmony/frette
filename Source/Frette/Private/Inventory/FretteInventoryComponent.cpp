#include "Inventory/FretteInventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

UFretteInventoryItem* UFretteInventoryComponent::GetItem(int32 Index)
{
	return Inventory.GetEntry(Index);
}

bool UFretteInventoryComponent::SelectItem(int32 Index)
{
	if (const UFretteInventoryItem* Item = GetItem(Index))
	{
		OnItemSelected.Broadcast(Item);
		return true;
	}

	return false;
}

void UFretteInventoryComponent::AddItem_Implementation(UFretteInventoryItemDataAsset* Template)
{
	UFretteInventoryItem* Item = Template->CreateRuntimeItem(this);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::RemoveItem(int32 Index)
{
	unimplemented(); // TODO:
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FFretteInventoryListEntry& Entry : Inventory.Entries)
		{
			UFretteInventoryItem* Instance = Entry.Item;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

void UFretteInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFretteInventoryComponent, Inventory, COND_OwnerOnly);
}

bool UFretteInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const FFretteInventoryListEntry& Entry : Inventory.Entries)
	{
		if (Entry.Item)
		{
			bWroteSomething |= Channel->ReplicateSubobject(Entry.Item, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}