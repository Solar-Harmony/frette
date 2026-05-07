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
	
	// Create an L-shaped path: Move exclusively UP first, then strictly FORWARD.
	// This ensures the capsule bottom fully clears the ledge corner before moving into the wall horizontally,
	// entirely eliminating the climbing body intersection.
	float UpAlpha = FMath::Clamp(Alpha * 2.0f, 0.0f, 1.0f);
	float ForwardAlpha = FMath::Clamp((Alpha - 0.5f) * 2.0f, 0.0f, 1.0f);
	
	FVector NewLocation;
	NewLocation.Z = FMath::Lerp(StartLocation.Z, EndLocation.Z, UpAlpha);
	NewLocation.X = FMath::Lerp(StartLocation.X, EndLocation.X, ForwardAlpha);
	NewLocation.Y = FMath::Lerp(StartLocation.Y, EndLocation.Y, ForwardAlpha);
	
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
