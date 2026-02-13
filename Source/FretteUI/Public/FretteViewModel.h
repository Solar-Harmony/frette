#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/FrettePlayerState.h"
#include "FretteViewModel.generated.h"

UCLASS(Abstract)
class FRETTEUI_API UFretteViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	virtual void Bind() {}

	UPROPERTY()
	TObjectPtr<AFrettePlayerState> PlayerState;

	UFUNCTION(BlueprintCallable, Category = "Frette|UI", meta = (DisplayName = "Create Frette View Model", DefaultToSelf = "Outer", DeterminesOutputType = "ViewModelClass"))
	static UFretteViewModel* CreateFretteViewModel(UObject* Outer, UPARAM(meta=(AllowAbstract = false)) TSubclassOf<UFretteViewModel> ViewModelClass, UPARAM(meta=(DisplayName = "Player State")) AFrettePlayerState* InPlayerState)
	{
		UFretteViewModel* ViewModel = NewObject<UFretteViewModel>(Outer, ViewModelClass);
		ViewModel->PlayerState = InPlayerState;
		ViewModel->Bind();
		return ViewModel;
	}
};