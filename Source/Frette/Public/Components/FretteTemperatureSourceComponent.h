#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "Components/DrawSphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Core/FretteWorldSettings.h"
#include "FretteTemperatureSourceComponent.generated.h"

UENUM(BlueprintType)
enum class ETemperatureSourceArrowRole : uint8
{
	None UMETA(DisplayName = "Hidden"),
	Temperature UMETA(DisplayName = "Temperature Field"),
	Flow UMETA(DisplayName = "Flow Field")
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature")
	float SourceRadius = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature")
	float DiffusionRadius = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float SourceTemperature = 100;

	// TODO Plug that into weather system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float AmbientTemperature = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta=(ClampMin="0.0", ClampMax="2.0",
		ToolTip="Multiplier applied to the temperature gradient when computing heat flow. Higher values increase how strongly heat propagates outward from the source. The best way to tune it is to use show numbers and flow arrows."))
	float FlowStrength = 1;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Temperature", meta=(ClampMin = "0.0", ClampMax = "1.0",
		ToolTip="This multiplier reduces the temperature contribution of this source to body parts that have no clear line of sight to the source radius."))
	float ObstructionFactor = 0.1;

	UFUNCTION(BlueprintCallable)
	float ComputeTemperature(float r) const;

	UFUNCTION(BlueprintCallable)
	float ComputeFlow(float r) const;

	UPROPERTY()
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY()
	TSet<TObjectPtr<ACharacter>> OverlappingCharacters;
	
	UPROPERTY()
	TObjectPtr<AFretteWorldSettings> WorldSettings;

	#if WITH_EDITORONLY_DATA
	void UpdateDebug();
	void UpdateMaterial() const;
	void UpdateDebugArrows();

	UPROPERTY()
	TArray<TObjectPtr<UArrowComponent>> DebugArrows;

	UPROPERTY()
	TObjectPtr<UDrawSphereComponent> DebugSphereInner;

	UPROPERTY()
	TObjectPtr<UTextRenderComponent> DebugText;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> SphereMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> FloorDiskMesh;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> HeatMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> HeatMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta=(ClampMin = "0.0", ClampMax = "1.0"))
	float RadialSlice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature")
	int NumberArrows = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature")
	ETemperatureSourceArrowRole ShowArrows = ETemperatureSourceArrowRole::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature")
	bool bShowNumbersAtSlice = false;

	FGuid UniqueId;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
};