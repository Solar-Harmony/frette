#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewModels/Equipment/FretteSlotsInventoryVM.h"
#include "FretteInventoryWidget.generated.h"

UCLASS()
class FRETTEUI_API UFretteInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette", meta=(ExposeOnSpawn))
	TObjectPtr<UFretteSlotsInventoryVM> SlotsVM;
	
private:
	virtual void NativeConstruct() override;
};