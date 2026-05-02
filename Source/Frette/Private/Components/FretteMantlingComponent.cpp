#include "Components/FretteMantlingComponent.h"

#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

void UFretteMantlingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<AFretteBaseCharacter>(GetOwner());
	if (Player == nullptr)
	{
		// todo use frettelog
		UE_LOG(LogTemp, Warning, TEXT("FretteMantlingComponent: Owner is not AFretteBaseCharacter"));
		bCanMantle = false;
	}
}

TOptional<FHitResult> UFretteMantlingComponent::DetectWall() const
{
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorForwardVector() * MaxDistanceToWall;
	const FCollisionShape Shape = Player->GetCapsuleComponent()->GetCollisionShape();
	
	// todo: check surface normal?
	if (FHitResult Hit; GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Shape))
		return Hit;
	
	return {};
}

TOptional<FHitResult> UFretteMantlingComponent::DetectLedge(const FHitResult& Wall) const
{
	const FVector Start = Wall.ImpactPoint;
	const FVector End = Start + FVector(0.0f, 0.0f, MaxObstacleHeight);
	const FCollisionShape Shape = FCollisionShape::MakeSphere(MaxLedgeGrabDistance);

	if (TArray<FHitResult> Hits; GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Visibility, Shape))
		return Hits.Last(); // todo: might be wrong
	
	return {};
}

bool UFretteMantlingComponent::HasEnoughSpaceAbove() const
{
	const FCollisionShape PlayerShape = Player->GetCapsuleComponent()->GetCollisionShape();
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorUpVector() * MaxObstacleHeight;
	return !GetWorld()->SweepTestByChannel(Start, End, FQuat::Identity, ECC_Visibility, PlayerShape);
}

bool UFretteMantlingComponent::FitsInSpace(const FVector& Location) const
{
	const FCollisionShape Shape = Player->GetCapsuleComponent()->GetCollisionShape();
	if (GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, ECC_Visibility, Shape))
		return true;
	
	return false;
}

void UFretteMantlingComponent::TryMantling() const
{
	if (!bCanMantle)
		return;

	TOptional<FHitResult> Wall = DetectWall();
	if (!Wall) 
		return;
	
	TOptional<FHitResult> Ledge = DetectLedge(*Wall);
	if (!Ledge)
		return;
	
	if (!FitsInSpace(Ledge->ImpactPoint))
		return;
	
	if (!HasEnoughSpaceAbove())
		return;
	
	Player->SetActorLocation(Ledge->ImpactPoint);
}