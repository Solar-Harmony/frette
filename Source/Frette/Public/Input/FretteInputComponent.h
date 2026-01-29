

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "FretteInputConfig.h"
#include "FretteInputComponent.generated.h"

struct FFretteInputAction;
/**
 * 
 */
UCLASS()
class FRETTE_API UFretteInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	public:
public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UFretteInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
	
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UFretteInputComponent::BindAbilityActions(const UFretteInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FFretteInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}
	}
}
