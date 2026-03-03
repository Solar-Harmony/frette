#pragma once

#include "CoreMinimal.h"
#include "Components/DrawSphereComponent.h"
#include "FretteMainObjective.generated.h"

/**
 * The treasure players are looking for.
 */
UCLASS()
class AFretteMainObjective : public AActor
{
	GENERATED_BODY()

public:
	AFretteMainObjective();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frette", meta=(DisplayName = "Radius to consider Near Objective", Units="cm"))
	float NearObjectiveRadiusCm = 1000.0f;
	
	virtual void BeginPlay() override;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDrawSphereComponent> DebugSphere;
#endif
};