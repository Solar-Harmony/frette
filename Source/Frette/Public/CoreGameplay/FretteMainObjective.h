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
		
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Frette", meta=(DisplayName = "Radius to consider Near Objective", Units="cm"))
	float NearObjectiveRadiusCm = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	// Name used in clues.
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UDrawSphereComponent> DebugSphere;
#endif
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};