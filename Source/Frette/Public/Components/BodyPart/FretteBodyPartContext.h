#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FretteBodyPartContext.generated.h"

USTRUCT(BlueprintType)
struct FFretteBodyPartContext
{
	GENERATED_BODY()

	UPROPERTY()
	float ValueDelta = 0.f;

	UPROPERTY()
	int AccumulatedValue = 0;

	UPROPERTY()
	FGameplayTag EffectType;
};