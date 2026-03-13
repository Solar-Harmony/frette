#pragma once

#include "CoreMinimal.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteRemainingHealthRule.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class FRETTE_API UFretteRemainingHealthRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::RemainingHealth; }

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;
};