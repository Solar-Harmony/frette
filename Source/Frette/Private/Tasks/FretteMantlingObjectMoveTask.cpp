#include "Tasks/FretteMantlingObjectMoveTask.h"
#include "GameFramework/Character.h"
#include "Character/FretteBaseCharacter.h"
#include "Components/PrimitiveComponent.h"

UFretteMantlingObjectMoveTask* UFretteMantlingObjectMoveTask::MantlingMoveToComponent(UGameplayAbility* OwningAbility, FName TaskInstanceName, UPrimitiveComponent* TargetComponent, FVector TargetRelativeLocation, float Duration)
{
	UFretteMantlingObjectMoveTask* MyObj = NewAbilityTask<UFretteMantlingObjectMoveTask>(OwningAbility, TaskInstanceName);
	MyObj->TargetComp = TargetComponent;
	MyObj->RelativeEndLocation = TargetRelativeLocation;
	MyObj->TotalDuration = FMath::Max(Duration, 0.01f);
	MyObj->bTickingTask = true;
	return MyObj;
}

void UFretteMantlingObjectMoveTask::Activate()
{
	Super::Activate();
	TimePassed = 0.0f;
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActor()))
	{
		StartLocation = Character->GetActorLocation();
	}
}

void UFretteMantlingObjectMoveTask::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (!Character || !IsValid(TargetComp))
	{
		EndTask();
		return;
	}

	TimePassed += DeltaTime;
	const float Alpha = FMath::Clamp(TimePassed / TotalDuration, 0.0f, 1.0f);
	
	const FVector EndLocation = TargetComp->GetComponentTransform().TransformPosition(RelativeEndLocation);
	const FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	
	Character->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (AFretteBaseCharacter* FretteCharacter = Cast<AFretteBaseCharacter>(Character))
	{
		const float DistanceToLedge = FVector::Dist(NewLocation, EndLocation);
		const float BlendDist = 100.0f;
		
		FretteCharacter->MantlingIKSnapAlpha = FMath::Clamp(1.0f - (DistanceToLedge - BlendDist) / BlendDist, 0.0f, 1.0f);
	}

	if (Alpha >= 1.0f)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast();
		}
		EndTask();
	}
}
