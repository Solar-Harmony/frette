#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "Components/DrawSphereComponent.h"
#include "FretteTemperatureSourceComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureSourceComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UFretteTemperatureSourceComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature")
	float InnerRadius = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature")
	float OuterRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature")
	float SourceTemperature = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature")
	float AmbientTemperature = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature", meta=(ClampMin = "0.0", ClampMax = "2.0"))
	float DiffusionStrength = 1.f; // Alpha in the heat equation
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature", meta=(ClampMin = "0.0", ClampMax = "1.0"))
	float VisualisationSlice = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature")
	int NumberFlowArrows = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Temperature")
	bool AroowsAreForTemperature = false;

	UFUNCTION(BlueprintCallable)
	float ComputeFalloff(float r) const;
	
	UFUNCTION(BlueprintCallable)
	float ComputeTemperature(float r) const;
	
	UFUNCTION(BlueprintCallable)
	float ComputeFlow(float r) const;

	UPROPERTY()
	USphereComponent* OverlapSphere;

	UPROPERTY()
	TSet<TObjectPtr<ACharacter>> OverlappingCharacters;
	
	#if WITH_EDITORONLY_DATA
	void UpdateMaterial();
	
	void UpdateDebugArrows();
	
	UPROPERTY()
	TArray<UArrowComponent*> DebugDiffusionArrows;

	UPROPERTY()
	TObjectPtr<UDrawSphereComponent> DebugSphereInner;
	
	UPROPERTY()
	UStaticMeshComponent* SphereMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* HeatMaterialInstance;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* HeatMaterial;
	
	FGuid UniqueId;
	float MinTemperature = -40;
	float MaxTemperature = 1500;
	
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	#endif
	
};