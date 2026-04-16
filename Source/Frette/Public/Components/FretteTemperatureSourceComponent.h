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
	void OnRegister();

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

	UFUNCTION(BlueprintCallable)
	float ComputeTemperature(float r) const;

	UPROPERTY()
	USphereComponent* OverlapSphere;

	UPROPERTY()
	TObjectPtr<UDrawSphereComponent> DebugSphereInner;
	
	#if WITH_EDITORONLY_DATA
	UFUNCTION(BlueprintCallable)
	void UpdateMaterial();

	UPROPERTY()
	UStaticMeshComponent* SphereMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* HeatMaterialInstance;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* HeatMaterial;
	
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	#endif
};