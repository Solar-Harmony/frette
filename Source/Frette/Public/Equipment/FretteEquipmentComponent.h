#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "FretteEquipmentComponent.generated.h"

class AFrettePlayerState;

UCLASS()
class FRETTE_API UFretteEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(const AFrettePlayerState* PlayerState);

private:
	void OnInventoryItemSelected(const UFretteInventoryItem* Item);
};