#include "Components/FretteGameplayStatics.h"

ECardinalDirection UFretteGameplayStatics::DirVectorToCardinal(const FVector2D& Dir)
{
	check(!Dir.IsNearlyZero());

	const float Angle = FMath::Atan2(Dir.Y, Dir.X); // get vector angle between -180 and 180 deg
	constexpr float SectorSize = PI / 4; // divide circle in 8 sectors, 45 deg each
	const int Sector = FMath::RoundToInt(Angle / SectorSize); // round to nearest sector -> [-4, 4]
	const int SectorIdx = (Sector % 8 + 8) % 8; // remap to [0, 7]

	return static_cast<ECardinalDirection>(SectorIdx);
}

bool UFretteGameplayStatics::ProjectActorToFloor(AActor* Actor, USceneComponent* SnapRoot, float ZOffset, FCollisionProfileName CollisionProfile)
{
	if (!IsValid(Actor) || !IsValid(SnapRoot))
		return false;
	
	TArray<USceneComponent*> Components;
	Components.Add(SnapRoot);
	SnapRoot->GetChildrenComponents(true, Components);

	FBox CombinedBounds(ForceInit);
	for (const USceneComponent* Component : Components)
	{
		if (!ensure(Component))
			continue;

		CombinedBounds += Component->CalcBounds(Component->GetComponentTransform()).GetBox();
	}
	
	const FVector Origin = Actor->GetActorLocation(); 
	const FVector TraceStart = FVector(Origin.X, Origin.Y, Origin.Z + 10000.0f); 	
	const FVector TraceEnd = FVector(Origin.X, Origin.Y, -HALF_WORLD_MAX);
	
	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ProjectActorToFloor), false, Actor);
	if (!Actor->GetWorld()->LineTraceSingleByProfile(Hit, TraceStart, TraceEnd, CollisionProfile.Name, QueryParams))
		return false;

	const float NewZ = Origin.Z + Hit.Location.Z - CombinedBounds.Min.Z + ZOffset;
	const FVector NewLocation = FVector(Origin.X, Origin.Y, NewZ);
	Actor->SetActorLocation(NewLocation);

	return true;
}