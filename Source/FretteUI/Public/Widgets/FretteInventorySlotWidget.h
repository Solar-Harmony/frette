#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FretteInventorySlotWidget.generated.h"

class UFretteSlotsInventorySlotVM;

UCLASS()
class FRETTEUI_API UFretteInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Frette")
	int32 SlotID;

private:
	virtual void NativeConstruct() override;
};