#pragma once

#include "CoreMinimal.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteSingleDamageInstanceRule.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class FRETTE_API UFretteSingleDamageInstanceRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::InstantDamage; }

	UPROPERTY(EditDefaultsOnly)
	int DamageThreshold;

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;

};