#include "Components/BodyPart/EffectRules/FretteEffectStackRule.h"

bool UFretteEffectStackRule::CheckCondition(FFretteBodyPartContext Context)
{
	const int32 CurrentStacks = Context.CumulativeValue;

	switch (Comparison)
	{
		case EStackThresholdComparison::GreaterThanOrEqual:
			return CurrentStacks >= StackAmountThreshold;
		case EStackThresholdComparison::LessThanOrEqual:
			return CurrentStacks <= StackAmountThreshold;
		default:
			return false;
	}
}