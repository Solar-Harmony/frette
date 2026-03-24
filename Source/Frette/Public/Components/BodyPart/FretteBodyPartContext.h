#pragma once
#include "CoreMinimal.h"
#include "FretteBodyPartData.h"
#include "GameplayTagContainer.h"
#include "FretteBodyPartContext.generated.h"

class UFretteBodyPartData;

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

	UPROPERTY()
	UFretteBodyPartData* SourceData = nullptr;
};