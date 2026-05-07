#include "Components/FretteMantlingAbility.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tasks/FretteMantlingObjectMoveTask.h"

static TAutoConsoleVariable CVarMantlingDebug(
	TEXT("Frette.Mantling.Debug"),
	false,
	TEXT("Show debug drawing for mantling.\n"),
	ECVF_Cheat);

static constexpr float DebugDrawDuration = 5.0f;
static constexpr float DebugDrawThickness = 2.0f;
static constexpr float MinObstacleHeight = 50.0f;
static constexpr float CapsuleRadiusShrinkScale = 0.8f;
static constexpr float CapsuleHeightShrinkScale = 0.9f;

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

	const bool bMantlingSuccessful = TryMantling(Player);
	if (!bMantlingSuccessful)
	{
		Player->Jump();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	} 
}

void UFretteMantlingAbility::OnMoveCompleted()
{
	if (AFretteBaseCharacter* Player = Cast<AFretteBaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		Player->MantlingIKSnapAlpha = 0.0f;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UFretteMantlingAbility::TryMantling(AFretteBaseCharacter* Player)
{
	TOptional<FHitResult> Wall = DetectWall(Player);
	if (!Wall) 
	{
		return false;
	}
	
	const float PlayerToWallAlignment = FVector::DotProduct(Player->GetActorForwardVector(), -Wall->ImpactNormal);
	if (PlayerToWallAlignment < 0.5f)
	{
		return false;
	}

	if (Wall->ImpactNormal.Z >= Player->GetCharacterMovement()->GetWalkableFloorZ())
	{
		return false;
	}
	
	TOptional<FHitResult> Ledge = DetectLedge(Player, *Wall);
	if (!Ledge)
	{
		return false;
	}
	
	const float CharacterBottom = Player->GetActorLocation().Z - Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const bool bIsLedgeHighEnough = Ledge->ImpactPoint.Z > CharacterBottom + MinObstacleHeight;
	if (!bIsLedgeHighEnough)
	{
		return false;
	}
	
	FVector TargetLocation = Ledge->ImpactPoint + FVector(0.0f, 0.0f, Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0f);

	UPrimitiveComponent* LedgeComp = Ledge->GetComponent();
	if (!LedgeComp)
	{
		return false;
	}

	if (!FitsInSpace(Player, TargetLocation))
	{
		return false;
	}
	
	if (!HasEnoughSpaceAbove(Player))
	{
		return false;
	}

	const FVector TargetRelativeLocation = LedgeComp->GetComponentTransform().InverseTransformPosition(TargetLocation);
	
	const FVector RightDir = Player->GetActorRightVector();
	const float HandOffset = 25.0f;
	Player->LeftHandMantlingIK = Ledge->ImpactPoint - RightDir * HandOffset;
	Player->RightHandMantlingIK = Ledge->ImpactPoint + RightDir * HandOffset;

	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugPoint(GetWorld(), Player->LeftHandMantlingIK, 25.0f, FColor::Yellow, false, DebugDrawDuration);
		DrawDebugPoint(GetWorld(), Player->RightHandMantlingIK, 25.0f, FColor::Yellow, false, DebugDrawDuration);
	}

	UFretteMantlingObjectMoveTask* MoveTask = UFretteMantlingObjectMoveTask::MantlingMoveToComponent(
		this, FName("MantlingMoveTask"), LedgeComp, TargetRelativeLocation, MantlingDuration);
	
	if (!MoveTask)
	{
		return false;
	}

	MoveTask->OnCompleted.AddDynamic(this, &UFretteMantlingAbility::OnMoveCompleted);
	MoveTask->ReadyForActivation();
	
	return true;
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
	const float CapsuleRadius = Player->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector WallForward = FVector(-Wall.Normal.X, -Wall.Normal.Y, 0.0f).GetSafeNormal();
	
	// Dynamically calculate the horizontal depth needed to clear the slanted wall geometry
	const float SlopePush = FMath::Max(0.0f, MaxObstacleHeight * (Wall.Normal.Z / FMath::Max(0.001f, Wall.Normal.Size2D())));
	const FVector Offset = WallForward * (CapsuleRadius + SlopePush + 5.0f);
	
	const FVector Start = Wall.ImpactPoint + FVector(0.0f, 0.0f, MaxObstacleHeight) + Offset;
	const FVector End = Wall.ImpactPoint + Offset;
	const FCollisionShape Shape = FCollisionShape::MakeSphere(CapsuleRadius);

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

bool UFretteMantlingAbility::FitsInSpace(const AFretteBaseCharacter* Player, const FVector& Location) const
{
	FCollisionShape Shape = Player->GetCapsuleComponent()->GetCollisionShape();
	Shape.SetCapsule(Shape.GetCapsuleRadius() * CapsuleRadiusShrinkScale, Shape.GetCapsuleHalfHeight() * CapsuleHeightShrinkScale);
	const bool bHit = GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, ECC_Visibility, Shape);
	
	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugCapsule(GetWorld(), Location, Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), FQuat::Identity, bHit ? FColor::Red : FColor::Green, false, DebugDrawDuration, 0, DebugDrawThickness);
	}

	return !bHit;
}

bool UFretteMantlingAbility::HasEnoughSpaceAbove(const AFretteBaseCharacter* Player) const
{
	FCollisionShape PlayerShape = Player->GetCapsuleComponent()->GetCollisionShape();
	PlayerShape.SetCapsule(PlayerShape.GetCapsuleRadius() * CapsuleRadiusShrinkScale, PlayerShape.GetCapsuleHalfHeight() * CapsuleHeightShrinkScale);
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorUpVector() * PlayerShape.GetCapsuleHalfHeight() * 2.0f;

	const bool bHit = GetWorld()->SweepTestByChannel(Start, End, FQuat::Identity, ECC_Visibility, PlayerShape);
	
	if (CVarMantlingDebug.GetValueOnAnyThread())
	{
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 5.0f, bHit ? FColor::Red : FColor::Green, false, DebugDrawDuration, 0, DebugDrawThickness);
	}

	return !bHit;
}
