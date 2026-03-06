#include "CoreGameplay/FretteClue.h"

#include "CoreGameplay/FretteClueTemplateSet.h"
#include "CoreGameplay/FretteGameMode.h"
#include "Frette/Frette.h"
#include "Inventory/Items/Impl/FretteClueItem.h"
#include "Kismet/GameplayStatics.h"

void AFretteClue::OnPickUp_Implementation(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem)
{
	require(ItemData.IsA<UFretteClueItemDataAsset>());
	require(IsValid(ClueTemplate));

	AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const FText ClueText = GameMode->GenerateClue(Interactor, ClueTemplate, DudClueChance);
	UFretteClueItem* ClueItem = CastChecked<UFretteClueItem>(AddedItem);
	ClueItem->ClueText = ClueText;
	ClueItem->GetOwningInventory()->ChangeItem_Implementation(ClueItem);

	AFrettePlayerController* PlayerController = CastChecked<AFrettePlayerController>(Interactor->GetController());
	PlayerController->Client_OnClueGenerated(ClueText);
}
