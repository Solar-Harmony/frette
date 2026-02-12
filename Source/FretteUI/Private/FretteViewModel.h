#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/FrettePlayerState.h"
#include "FretteViewModel.generated.h"

UCLASS(Abstract)
class FRETTEUI_API UFretteViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	virtual void Bind(AFrettePlayerState* PlayerState) {}

	UFUNCTION(BlueprintCallable, Category = "Frette|UI", meta = (DisplayName = "Create Frette View Model", DefaultToSelf = "Outer"))
	static UFretteViewModel* CreateFretteViewModel(UObject* Outer, TSubclassOf<UFretteViewModel> ViewModelClass, AFrettePlayerState* InPlayerState)
	{
		UFretteViewModel* ViewModel = NewObject<UFretteViewModel>(Outer, ViewModelClass);
		ViewModel->Bind(InPlayerState);
		return ViewModel;
	}
};