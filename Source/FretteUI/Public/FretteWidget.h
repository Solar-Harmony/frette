#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Misc/DataValidation.h"
#include "FretteWidget.generated.h"

UCLASS(Abstract)
class FRETTEUI_API UFretteWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// allow implementing validation in widget blueprints
	UFUNCTION(BlueprintNativeEvent, meta=(DisplayName = "Validate", DevelopmentOnly = true))
	bool K2_Validate() const;
	bool K2_Validate_Implementation() const { return true; }
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		if (K2_Validate())
			return EDataValidationResult::Valid;
		
		Context.AddError(INVTEXT("Frette widget blueprint validation failed."));
		return EDataValidationResult::Invalid;
	}
#endif // WITH_EDITOR
};