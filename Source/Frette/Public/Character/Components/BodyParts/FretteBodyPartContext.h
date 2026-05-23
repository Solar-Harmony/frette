#pragma once
#include "CoreMinimal.h"
#include "Character/Components/BodyParts/FretteBodyPartData.h"
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
	float AccumulatedValue = 0.f;

	UPROPERTY()
	FGameplayTag EffectType;

	UPROPERTY()
	UFretteBodyPartData* SourceData = nullptr;
};