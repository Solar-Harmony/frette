#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FretteWidget.generated.h"

class UFretteViewModel;

UCLASS()
class FRETTEUI_API UFretteWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static UFretteWidget* CreateFretteWidget(APlayerController* OwningPlayer, TSubclassOf<UFretteWidget> WidgetClass, TSubclassOf<UFretteViewModel> ViewModelClass);
	
	UPROPERTY(Transient)
	TObjectPtr<UFretteViewModel> ViewModel;
};