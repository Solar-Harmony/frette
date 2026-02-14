#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "FretteDamageEffectExecCalc.generated.h"

UCLASS()
class FRETTE_API UFretteDamageEffectExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
