#pragma once

#include "CoreMinimal.h"
#include "Character/FrettePlayerCharacter.h"
#include "CoreGameplay/FretteGameMode.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FrettePlayerController.h"
#include "FretteObjectiveProbeItem.generated.h"

class UFretteObjectiveProbeItemDataAsset;

UCLASS(BlueprintType)
class UFretteObjectiveProbeItem : public UFretteSlottableItem
{
	GENERATED_BODY()
	FRETTE_ITEM_DATA_GETTER(UFretteObjectiveProbeItemDataAsset)

public:
	virtual void Use_Implementation() override
	{
		AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		GameMode->ProbeForObjective(CastChecked<AFrettePlayerCharacter>(GetOwningPlayer()));
	}
};

UCLASS()
class UFretteObjectiveProbeItemDataAsset : public UFretteSlottableItemDataAsset
{
	GENERATED_BODY()
	
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteObjectiveProbeItem::StaticClass(); }
};