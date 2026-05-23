#include "Character/Components/BodyParts/EffectRules/FretteAccumulatedValueRule.h"

bool UFretteAccumulatedValueRule::CheckCondition(FFretteBodyPartContext Context)
{
	const float CurrentStacks = Context.AccumulatedValue;

	switch (Comparison)
	{
		case EValueComparison::GreaterThanOrEqual:
			return CurrentStacks >= TotalValueThreshold;
		case EValueComparison::LessThanOrEqual:
			return CurrentStacks <= TotalValueThreshold;
		default:
			return false;
	}
}