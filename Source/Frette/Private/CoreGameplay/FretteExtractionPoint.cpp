#include "CoreGameplay/FretteExtractionPoint.h"

#include "Character/FrettePlayerCharacter.h"
#include "CoreGameplay/FretteGameMode.h"
#include "Inventory/FretteInventoryComponent.h"

void AFretteExtractionPoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const AFrettePlayerCharacter* PlayerCharacter = Cast<AFrettePlayerCharacter>(OtherActor);
	if (PlayerCharacter == nullptr)
		return;
	
	if (HasAuthority())
	{
		const AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->CheckVictory(PlayerCharacter);
	}
}