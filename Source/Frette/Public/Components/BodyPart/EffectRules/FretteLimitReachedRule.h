#pragma once

#include "CoreMinimal.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteLimitReachedRule.generated.h"

UENUM(BlueprintType)
enum class EValueLimitType : uint8
{
	Min,
	Max
};

//Pourrais être gerer juste avec les accumulated values mais utiliser LimitReached permet 
//de ne pas avoir a changer les regles si jamais un min ou un max change pour une valeur
UCLASS(EditInlineNew)
class FRETTE_API UFretteLimitReachedRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::LimitReached; }

	UPROPERTY(EditDefaultsOnly)
	EValueLimitType LimitToCheck = EValueLimitType::Min;

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;
};