#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Misc/DataValidation.h"
#include "FretteViewModel.generated.h"

UCLASS(Abstract)
class FRETTEUI_API UFretteViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	virtual void Bind(APlayerController* OwningController) {}
	
private:
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		const UClass* Class = GetClass();
		
		if (Class->IsInBlueprint() && !Class->IsFunctionImplementedInScript("Bind"))
		{
			const FText Error = FText::Format(INVTEXT("ViewModel '{0}' does not implement a Bind() event."), FText::FromString(GetName()));
			Context.AddError(Error);
			return EDataValidationResult::Invalid;
		}
		
		return EDataValidationResult::Valid;
	}
};

