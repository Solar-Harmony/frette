#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "FretteEquipmentComponent.generated.h"

class AFrettePlayerState;

UCLASS()
class FRETTE_API UFretteEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void EquipItem(UFretteSlottableItem* Item) const;

	UFUNCTION(Server, Reliable)
	void UnequipItem(UFretteSlottableItem* Item) const;
};