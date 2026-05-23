#include "Character/Components/BodyParts/EffectRules/FretteLimitReachedRule.h"

bool UFretteLimitReachedRule::CheckCondition(const FFretteBodyPartContext Context)
{
	switch (LimitToCheck)
	{
		case EValueLimitType::Min:
			return Context.AccumulatedValue == Context.SourceData->GetMinValueForType(Context.EffectType);

		case EValueLimitType::Max:
			return Context.AccumulatedValue == Context.SourceData->GetMaxValueForType(Context.EffectType);
	}

	return false;
}