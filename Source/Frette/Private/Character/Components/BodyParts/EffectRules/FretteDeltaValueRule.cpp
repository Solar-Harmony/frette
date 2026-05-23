#include "Character/Components/BodyParts/EffectRules/FretteDeltaValueRule.h"

bool UFretteDeltaValueRule::CheckCondition(FFretteBodyPartContext Context)
{
	return Context.ValueDelta >= Threshold;
}