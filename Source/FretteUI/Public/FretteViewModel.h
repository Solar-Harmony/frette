#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Character/FrettePlayerCharacter.h"
#include "FretteViewModel.generated.h"

UCLASS(Abstract)
class FRETTEUI_API UFretteViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	virtual void Bind() {}

	UPROPERTY()
	TObjectPtr<AFrettePlayerCharacter> PlayerCharacter;

	UFUNCTION(BlueprintCallable, Category = "Frette|UI", meta = (DisplayName = "Create Frette View Model", DefaultToSelf = "Outer", DeterminesOutputType = "ViewModelClass"))
	static UFretteViewModel* CreateFretteViewModel(UObject* Outer, UPARAM(meta=(AllowAbstract = false)) TSubclassOf<UFretteViewModel> ViewModelClass, UPARAM(meta=(DisplayName = "Player Character")) AFrettePlayerCharacter* InPlayerCharacter)
	{
		UFretteViewModel* ViewModel = NewObject<UFretteViewModel>(Outer, ViewModelClass);
		ViewModel->PlayerCharacter = InPlayerCharacter;
		ViewModel->Bind();
		return ViewModel;
	}
};