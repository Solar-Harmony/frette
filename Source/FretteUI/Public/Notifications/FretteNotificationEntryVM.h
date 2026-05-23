#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "FretteNotificationEntryVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteNotificationEntryVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	void SetMessage(const FText& Text)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Message, Text);
	}

protected:
	UPROPERTY(FieldNotify, BlueprintReadOnly)
	FText Message;
};