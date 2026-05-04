#include "Components/FretteMantlingAbility.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"

static TAutoConsoleVariable CVarMantlingDebug(
	TEXT("Frette.Mantling.Debug"),
	false,
	TEXT("Show debug drawing for mantling.\n"),
	ECVF_Cheat);

static constexpr float DebugDrawDuration = 5.0f;
static constexpr float DebugDrawThickness = 3.0f;

UFretteMantlingAbility::UFretteMantlingAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFretteMantlingAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AFretteBaseCharacter* Player = Cast<AFretteBaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!Player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TOptional<FHitResult> Wall = DetectWall(Player);
	if (!Wall) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	TOptional<FHitResult> Ledge = DetectLedge(Player, *Wall);
	if (!Ledge)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	const FVector TargetLocation = Ledge->ImpactPoint + FVector(0.0f, 0.0f, Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0f);

	if (!FitsInSpace(Player, TargetLocation))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	if (!HasEnoughSpaceAbove(Player))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	UAbilityTask_MoveToLocation* MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(
		this, FName("MantlingMoveTask"), TargetLocation, MantlingDuration, nullptr, nullptr);
	
	if (MoveTask)
	{
		MoveTask->OnTargetLocationReached.AddDynamic(this, &UFretteMantlingAbility::OnMoveCompleted);
		MoveTask->ReadyForActivation();
	}
	else
	{
		Player->SetActorLocation(TargetLocation);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UFretteMantlingAbility::OnMoveCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

TOptional<FHitResult> UFretteMantlingAbility::DetectWall(AFretteBaseCharacter* Player) const
{
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorForwardVector() * MaxDistanceToWall;
	const FCollisionShape Shape = Player->GetCapsuleComponent()->GetCollisionShape();
	
	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Shape);
	
	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 5.0f, bHit ? FColor::Green : FColor::Red, false, DebugDrawDuration, 0, DebugDrawThickness);
		if (bHit) DrawDebugCapsule(GetWorld(), Hit.ImpactPoint, Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), FQuat::Identity, FColor::Cyan, false, DebugDrawDuration, 0, DebugDrawThickness);
	}
	
	if (bHit)
		return Hit;
	
	return {};
}

TOptional<FHitResult> UFretteMantlingAbility::DetectLedge(AFretteBaseCharacter* Player, const FHitResult& Wall) const
{
	const FVector WallForward = -Wall.Normal;
	const FVector Start = Wall.ImpactPoint + FVector(0.0f, 0.0f, MaxObstacleHeight) + (WallForward * 20.0f);
	const FVector End = Wall.ImpactPoint + (WallForward * 20.0f);
	const FCollisionShape Shape = FCollisionShape::MakeSphere(MaxLedgeGrabDistance);

	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Shape);

	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, DebugDrawDuration, 0, DebugDrawThickness);
		if (bHit) DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Shape.GetSphereRadius(), 12, FColor::Purple, false, DebugDrawDuration, 0, DebugDrawThickness);
	}

	if (bHit)
		return Hit;
	
	return {};
}

bool UFretteMantlingAbility::FitsInSpace(AFretteBaseCharacter* Player, const FVector& Location) const
{
	const FCollisionShape Shape = Player->GetCapsuleComponent()->GetCollisionShape();
	const bool bHit = GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, ECC_Visibility, Shape);
	
	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugCapsule(GetWorld(), Location, Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), FQuat::Identity, bHit ? FColor::Red : FColor::Green, false, 2.0f);
	}

	return !bHit;
}

bool UFretteMantlingAbility::HasEnoughSpaceAbove(const AFretteBaseCharacter* Player) const
{
	const FCollisionShape PlayerShape = Player->GetCapsuleComponent()->GetCollisionShape();
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorUpVector() * MaxObstacleHeight;

	const bool bHit = GetWorld()->SweepTestByChannel(Start, End, FQuat::Identity, ECC_Visibility, PlayerShape);
	
	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 5.0f, bHit ? FColor::Red : FColor::Green, false, DebugDrawDuration, 0, DebugDrawThickness);
	}

	return !bHit;
}
