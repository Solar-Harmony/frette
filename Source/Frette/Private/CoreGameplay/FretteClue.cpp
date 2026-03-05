#include "CoreGameplay/FretteClue.h"

#include "CoreGameplay/FretteGameMode.h"
#include "Frette/Frette.h"
#include "Kismet/GameplayStatics.h"

void AFretteClue::OnPickUp_Implementation(AFrettePlayerCharacter* Interactor)
{
	AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	require(IsValid(GameMode), "Cannot generate clue because gamemode is not available. Clue pickup must happen on the server.")
	GameMode->GenerateClue(Interactor, DudClueChance, Steepness, Midpoint);
}
