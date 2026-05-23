#include "Game/Treasure/FretteExtractionPoint.h"

#include "Character/Player/FrettePlayerCharacter.h"
#include "Game/FretteGameMode.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Kismet/GameplayStatics.h"

void AFretteExtractionPoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const AFrettePlayerCharacter* PlayerCharacter = Cast<AFrettePlayerCharacter>(OtherActor);
	if (PlayerCharacter == nullptr)
		return;
	
	if (HasAuthority())
	{
		const AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		
		if (!GameMode->IsGameEnded())
		{
			GameMode->CheckVictory(PlayerCharacter);
		}
	}
}