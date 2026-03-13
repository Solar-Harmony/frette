#pragma once
#include "CoreMinimal.h"
#include "FretteBodyPartContext.generated.h"

USTRUCT(BlueprintType)
struct FFretteBodyPartContext
{
	GENERATED_BODY()

	UPROPERTY()
	float InstantDamage = 0.f;

	UPROPERTY()
	float RemainingHealth = 0.f;

	UPROPERTY()
	int StackAmount = 0;
};