#pragma once

#include "CoreMinimal.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteDeltaValueRule.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class FRETTE_API UFretteDeltaValueRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::DeltaValue; }

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1"))
	int Threshold;

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;

};