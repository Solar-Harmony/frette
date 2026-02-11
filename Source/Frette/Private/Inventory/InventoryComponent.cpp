#include "Inventory/InventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

UInventoryItem* UFretteInventoryComponent::GetItem(int32 Index) const
{
	TArray<UInventoryItem*> Items = Inventory.GetAllItems(); // FIXME: dumb
	return Items.IsValidIndex(Index) ? Items[Index] : nullptr;
}

void UFretteInventoryComponent::AddItem_Implementation(UInventoryItemDataAsset* Template)
{
	UInventoryItem* Item = Template->CreateRuntimeItem(this);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::RemoveItem(int32 Index)
{
	unimplemented(); // TODO:
}

void UFretteInventoryComponent::SwapItems(int32 FromIndex, int32 ToIndex)
{
	unimplemented(); // TODO:
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FInventoryListEntry& Entry : Inventory.Entries)
		{
			UInventoryItem* Instance = Entry.Item;

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

	for (const FInventoryListEntry& Entry : Inventory.Entries)
	{
		if (Entry.Item)
		{
			bWroteSomething |= Channel->ReplicateSubobject(Entry.Item, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}