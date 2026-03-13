#pragma once

#include "CoreMinimal.h"
#include "FretteBodyPartContext.h"
#include "UObject/Object.h"
#include "FretteBodyPartEffectRule.generated.h"

UENUM(BlueprintType)
enum class EBodyPartEventType : uint8
{
	InstantDamage,
	RemainingHealth,
	Death,
	StatusEffect
};

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class FRETTE_API UFretteBodyPartEffectRule : public UObject
{
	GENERATED_BODY()

public:
	bool bHasTriggered = false;

	virtual EBodyPartEventType GetRelatedEvent() const { return {}; }

	bool CheckIfTriggers(const FFretteBodyPartContext& Context);

	void Reset() { bHasTriggered = false; }

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context);
};