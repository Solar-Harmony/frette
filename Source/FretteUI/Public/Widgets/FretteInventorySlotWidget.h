#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FretteInventorySlotWidget.generated.h"

class UFretteSlotsInventorySlotVM;

UCLASS()
class FRETTEUI_API UFretteInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Frette.Inventory.SlotType"))
	FGameplayTag CompatibleSlotType;
};