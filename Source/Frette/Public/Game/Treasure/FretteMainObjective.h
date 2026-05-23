#pragma once

#include "CoreMinimal.h"
#include "Components/DrawSphereComponent.h"
#include "Game/Treasure/FretteObjectiveItem.h"
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
	void SetCollected(bool bInCollected);
		
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Frette", meta=(DisplayName = "Near Objective Radius", Units="cm"))
	float NearObjectiveRadiusCm = 1000.0f;
	
	// radius considered for probing objective and winning
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Frette", meta=(DisplayName = "Right on Objective Radius", Units="cm"))
	float RightOnObjectiveRadiusCm = 200.0f;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UFretteObjectiveItemDataAsset> ObjectiveItemData;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	// Name used in clues.
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UDrawSphereComponent> DebugSphere;
	
	UPROPERTY()
	TObjectPtr<UDrawSphereComponent> DebugSphereInner;
#endif
	
protected:
	UFUNCTION()
	void OnRep_IsCollected();
	void ApplyCollectedState();

	UPROPERTY(ReplicatedUsing = OnRep_IsCollected)
	bool bIsCollected = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
#if WITH_EDITORONLY_DATA
	void CreateDebugSphere(UDrawSphereComponent* SphereComponent);
#endif
};
