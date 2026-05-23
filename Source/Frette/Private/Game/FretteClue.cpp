#include "Game/Clues/FretteClue.h"

#include "Game/Clues/FretteClueTemplateSet.h"
#include "Game/FretteGameMode.h"
#include "Frette.h"
#include "Game/Clues/FretteClueItem.h"
#include "Kismet/GameplayStatics.h"

void AFretteClue::OnPickUp_Implementation(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem)
{
	unless(ItemData.IsA<UFretteClueItemDataAsset>()) return;
	unless(IsValid(ClueTemplate)) return;

	AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const TPair<FText, bool> ClueText = GameMode->GenerateClue(Interactor, ClueTemplate);
	UFretteClueItem* ClueItem = CastChecked<UFretteClueItem>(AddedItem);
	ClueItem->ClueText = ClueText.Key;
	ClueItem->bIsPrimary = ClueText.Value;
	ClueItem->GetOwningInventory()->ChangeItem_Implementation(ClueItem);

	AFrettePlayerController* PlayerController = CastChecked<AFrettePlayerController>(Interactor->GetController());
	PlayerController->Client_OnClueGenerated(ClueText.Key);
}
