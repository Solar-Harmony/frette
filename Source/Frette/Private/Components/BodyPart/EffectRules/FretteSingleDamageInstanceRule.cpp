#include "Components/BodyPart/EffectRules/FretteSingleDamageInstanceRule.h"

bool UFretteSingleDamageInstanceRule::CheckCondition(FFretteBodyPartContext Context)
{
	return Context.InstantDamage >= DamageThreshold;
}