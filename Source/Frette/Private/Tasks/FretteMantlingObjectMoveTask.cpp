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
	
	// Create a continuous, dynamic climbing cure:
	// Pull up quickly to clear the edge, then shift body weight forward smoothly for momentum
	float UpAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);
	float ForwardAlpha = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, 2.0f);
	
	FVector NewLocation;
	NewLocation.Z = FMath::Lerp(StartLocation.Z, EndLocation.Z, UpAlpha);
	NewLocation.X = FMath::Lerp(StartLocation.X, EndLocation.X, ForwardAlpha);
	NewLocation.Y = FMath::Lerp(StartLocation.Y, EndLocation.Y, ForwardAlpha);
	
	Character->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (AFretteBaseCharacter* FretteCharacter = Cast<AFretteBaseCharacter>(Character))
	{
		FretteCharacter->MantlingIKSnapAlpha = 1.0f;
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
