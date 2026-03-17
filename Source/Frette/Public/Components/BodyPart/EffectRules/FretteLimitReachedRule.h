#pragma once

#include "CoreMinimal.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteLimitReachedRule.generated.h"

//Pas sur du nom mais c'est pour les limites genre min et max
UCLASS(EditInlineNew)
class FRETTE_API UFretteLimitReachedRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::LimitReached; }

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;
};