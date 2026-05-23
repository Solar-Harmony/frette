#pragma once

#include "CoreMinimal.h"
#include "Character/Components/BodyParts/FretteBodyPartContext.h"
#include "UObject/Object.h"
#include "FretteBodyPartEffectRule.generated.h"

UENUM(BlueprintType)
enum class EBodyPartEventType : uint8
{
	DeltaValue,
	LimitReached,
	AccumulatedValue
};

UENUM(BlueprintType)
enum class ETriggerBehaviour : uint8
{
	TriggerOnce,
	Repeat,
};

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class FRETTE_API UFretteBodyPartEffectRule : public UObject
{
	GENERATED_BODY()

public:
	bool bHasTriggered = false;
	virtual EBodyPartEventType GetRelatedEvent() const { return {}; }
	bool CheckIfTriggers(const FFretteBodyPartContext& Context);
	virtual bool CheckCondition(FFretteBodyPartContext Context);
	void Reset() { bHasTriggered = false; }

	UPROPERTY(EditAnywhere)
	ETriggerBehaviour TriggerBehaviour = ETriggerBehaviour::TriggerOnce;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Frette.BodyPartValues"))
	FGameplayTag TagType;

protected:
};