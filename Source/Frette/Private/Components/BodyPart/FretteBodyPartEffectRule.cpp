#include "Components/BodyPart/FretteBodyPartEffectRule.h"

bool UFretteBodyPartEffectRule::CheckIfTriggers(const FFretteBodyPartContext& Context)
{
	if (bHasTriggered)
		return false;

	if (CheckCondition(Context))
	{
		//Check si l'éffet peut être déclancher plusieurs fois ou pas, si pas 
		//alors on set bHasTriggered à true pour pas qu'il puisse être déclancher à nouveau
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