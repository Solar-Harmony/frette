#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/FretteGameplayStatics.h"
#include "FretteCompassComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteCompassComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteCompassComponent();
	
	UFUNCTION(BlueprintPure)
	ECardinalDirection GetPlayerCardinalDirection() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UCameraComponent> Camera = nullptr;
	
	ECardinalDirection CachedDirection = ECardinalDirection::North;
};

