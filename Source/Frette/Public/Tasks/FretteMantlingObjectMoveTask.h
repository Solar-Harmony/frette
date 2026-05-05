#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FretteMantlingObjectMoveTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMantlingMoveCompletedSignature);

UCLASS()
class UFretteMantlingObjectMoveTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FMantlingMoveCompletedSignature OnCompleted;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UFretteMantlingObjectMoveTask* MantlingMoveToComponent(UGameplayAbility* OwningAbility, FName TaskInstanceName, UPrimitiveComponent* TargetComponent, FVector TargetRelativeLocation, float Duration);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	UPROPERTY()
	UPrimitiveComponent* TargetComp;
	FVector StartLocation;
	FVector RelativeEndLocation;
	float TotalDuration;
	float TimePassed;
};
