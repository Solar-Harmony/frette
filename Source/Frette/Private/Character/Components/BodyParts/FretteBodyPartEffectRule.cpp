#include "Components/BodyPart/FretteBodyPartEffectRule.h"

bool UFretteBodyPartEffectRule::CheckIfTriggers(const FFretteBodyPartContext& Context)
{
	if (bHasTriggered)
		return false;

	if (CheckCondition(Context))
	{
		if (TriggerBehaviour == ETriggerBehaviour::TriggerOnce)
			bHasTriggered = true;

		return true;
	}

	return false;
}

bool UFretteBodyPartEffectRule::CheckCondition(FFretteBodyPartContext Context)
{
	ensureMsgf(false, TEXT("CheckIfTriggers not implemented in %s"), *GetClass()->GetName());
	return false;
}