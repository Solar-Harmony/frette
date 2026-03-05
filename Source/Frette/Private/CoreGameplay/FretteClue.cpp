#include "CoreGameplay/FretteClue.h"

#include "CoreGameplay/FretteGameMode.h"
#include "Frette/Frette.h"
#include "Inventory/Items/Impl/FretteClueItem.h"
#include "Kismet/GameplayStatics.h"

void AFretteClue::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	require(ItemData.IsA<UFretteClueItemDataAsset>());
}

void AFretteClue::OnPickUp_Implementation(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem)
{
	AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const FText ClueText = GameMode->GenerateClue(Interactor, DudClueChance, Steepness, Midpoint);
	UFretteClueItem* ClueItem = CastChecked<UFretteClueItem>(AddedItem);
	ClueItem->ClueText = ClueText;
	ClueItem->GetOwningInventory()->ChangeItem_Implementation(ClueItem);

	AFrettePlayerController* PlayerController = CastChecked<AFrettePlayerController>(Interactor->GetController());
	PlayerController->Client_OnClueGenerated(ClueText);
}
