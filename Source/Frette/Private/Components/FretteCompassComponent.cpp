#include "Components/FretteCompassComponent.h"
#include "Camera/CameraComponent.h"

UFretteCompassComponent::UFretteCompassComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

ECardinalDirection UFretteCompassComponent::GetPlayerCardinalDirection() const
{
	return CachedDirection;
}

void UFretteCompassComponent::BeginPlay()
{
	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
}

void UFretteCompassComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	const FVector Forward3D = Camera->GetForwardVector();
	const FVector2D Forward2D(Forward3D.X, Forward3D.Y);
	
	if (ensure(!Forward2D.IsNearlyZero()))
	{
		CachedDirection = UFretteGameplayStatics::DirVectorToCardinal(Forward2D.GetSafeNormal());
	}
}

