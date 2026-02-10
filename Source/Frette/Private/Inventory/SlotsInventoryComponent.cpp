#include "Inventory/SlotsInventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

USlotsInventoryComponent::USlotsInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

int32 USlotsInventoryComponent::GetItemCount(UInventoryItemDataAsset* ItemType)
{
	return 67; // TODO: implement
}

UInventoryItem* USlotsInventoryComponent::GetItem(int32 Index)
{
	TArray<UInventoryItem*> Items = Inventory.GetAllItems(); // FIXME: dumb
	return Items.IsValidIndex(Index) ? Items[Index] : nullptr;
}

void USlotsInventoryComponent::AddItem_Implementation(UInventoryItemDataAsset* ItemData)
{
	Inventory.AddEntry(ItemData, 1);
}

void USlotsInventoryComponent::RemoveItem(int32 Index)
{
	unimplemented(); // TODO:
}

void USlotsInventoryComponent::SwapItems(int32 FromIndex, int32 ToIndex)
{
	unimplemented(); // TODO:
}

void USlotsInventoryComponent::ReadyForReplication()
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

void USlotsInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USlotsInventoryComponent, Inventory);
}

bool USlotsInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
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