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

	UFUNCTION(BlueprintCallable, Category = "Frette|UI", meta = (DisplayName = "Create Frette View Model", DefaultToSelf = "Outer", DeterminesOutputType = "ViewModelClass"))
	static UFretteViewModel* CreateFretteViewModel(UObject* Outer, UPARAM(meta=(AllowAbstract = false)) TSubclassOf<UFretteViewModel> ViewModelClass, AFrettePlayerState* PlayerState)
	{
		UFretteViewModel* ViewModel = NewObject<UFretteViewModel>(Outer, ViewModelClass);
		ViewModel->Bind(PlayerState);
		return ViewModel;
	}
};