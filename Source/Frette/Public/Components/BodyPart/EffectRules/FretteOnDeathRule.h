#pragma once

#include "CoreMinimal.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteOnDeathRule.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class FRETTE_API UFretteOnDeathRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::Death; }

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;
};