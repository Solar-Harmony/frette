#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/BodyPart/FretteBodyPartEffectRule.h"
#include "FretteAccumulatedValueRule.generated.h"

UENUM(BlueprintType)
enum class EValueComparison : uint8
{
	GreaterThanOrEqual,
	LessThanOrEqual
};

UCLASS(EditInlineNew)
class FRETTE_API UFretteAccumulatedValueRule : public UFretteBodyPartEffectRule
{
	GENERATED_BODY()

public:
	virtual EBodyPartEventType GetRelatedEvent() const override { return EBodyPartEventType::AccumulatedValue; }

	UPROPERTY(EditDefaultsOnly)
	int TotalValueThreshold;

	//Vu que la température va être une seule valeur qui bouge entre -100 et 100,
	//On peut dire si on veut que des éffet de froid se déclance genre en dessous de -20 
	//ou des éffet de chaleur au dessus de 20
	UPROPERTY(EditDefaultsOnly)
	EValueComparison Comparison = EValueComparison::GreaterThanOrEqual;

protected:
	virtual bool CheckCondition(FFretteBodyPartContext Context) override;
};