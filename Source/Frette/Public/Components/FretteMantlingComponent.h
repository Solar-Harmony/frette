#pragma once

#include "CoreMinimal.h"
#include "Character/FretteBaseCharacter.h"
#include "FretteMantlingComponent.generated.h"

/*
 * Component to vault on top
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteMantlingComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void TryMantling() const;
	
	UPROPERTY(EditDefaultsOnly)
	bool bCanMantle = true;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxDistanceToWall = 50.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxLedgeGrabDistance = 50.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxObstacleHeight = 150.0f;
	
private:
	UPROPERTY()
	AFretteBaseCharacter* Player;
	
	TOptional<FHitResult> DetectWall() const;
	TOptional<FHitResult> DetectLedge(const FHitResult& Wall) const;
	bool HasEnoughSpaceAbove() const;
	bool FitsInSpace(const FVector& Location) const;
};
