#include "Inventory/FretteInventoryComponent.h"

#include "Frette/Frette.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommandWithWorldAndArgs CmdDumpInventory(
	TEXT("Frette.DumpInventory"),
	TEXT("Dumps the inventory contents of the local player's pawn. Usage: Frette.DumpInventory"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogFrette, Warning, TEXT("DumpInventory: Invalid world"));
			return;
		}

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC)
		{
			UE_LOG(LogFrette, Warning, TEXT("DumpInventory: No player controller found"));
			return;
		}

		APawn* Pawn = PC->GetPawn();
		if (!Pawn)
		{
			UE_LOG(LogFrette, Warning, TEXT("DumpInventory: Player controller has no pawn"));
			return;
		}

		UFretteInventoryComponent* InventoryComp = Pawn->FindComponentByClass<UFretteInventoryComponent>();
		if (!InventoryComp)
		{
			UE_LOG(LogFrette, Warning, TEXT("DumpInventory: Pawn has no inventory component"));
			return;
		}

		InventoryComp->DumpInventory();
	})
);
#endif

UFretteInventoryComponent::UFretteInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UFretteInventoryComponent::SelectItem(int32 ItemId) const
{
	const UFretteInventoryItem* ItemToSelect = GetItem(ItemId);
	require(ItemToSelect, "Inventory: Cannot select item #%d because it was not found in this inventory.", ItemId);

	OnItemSelected.Broadcast(ItemToSelect);
	K2_OnItemSelected.Broadcast(ItemToSelect);
}

void UFretteInventoryComponent::AddItem_Implementation(UFretteInventoryItemDataAsset* ItemData)
{
	require(IsReadyForReplication() && GetOwner()->HasAuthority());
	require(IsValid(ItemData), "Inventory: Cannot add item: item data asset is invalid.");

	UFretteInventoryItem* Item = ItemData->CreateRuntimeItem(this);
	AddReplicatedSubObject(Item);
	Inventory.AddEntry(Item);
}

void UFretteInventoryComponent::ChangeItem_Implementation(UFretteInventoryItem* ItemToChange)
{
	require(GetOwner()->HasAuthority());
	require(Inventory.IsValidItem(ItemToChange), "Inventory: Cannot change item because it is invalid.");
	Inventory.ChangeEntry(ItemToChange);
}

void UFretteInventoryComponent::RemoveItem_Implementation(int32 ItemId)
{
	UE_LOG(LogTemp, Warning, TEXT("Server RPC EXECUTED | NetMode: %d | Role: %d | RemoteRole: %d"),
		(int32)GetNetMode(),
		(int32)GetOwner()->GetLocalRole(),
		(int32)GetOwner()->GetRemoteRole());
	require(GetOwner()->HasAuthority());
w	require(Inventory.HasEntry(ItemId), "Inventory: Cannot remove item #%d because this inventory has no item with that ID.", ItemId);

	Inventory.RemoveEntry(ItemId);
}

void UFretteInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	for (TObjectPtr StartingItemData : StartingItems)
	{
		AddItem(StartingItemData);
	}
}

void UFretteInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UFretteInventoryComponent, Inventory, COND_OwnerOnly);
}