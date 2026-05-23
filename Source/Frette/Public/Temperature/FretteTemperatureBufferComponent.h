#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/FretteWorldSettings.h"
#include "FretteTemperatureBufferComponent.generated.h"

struct FPropertyChangedEvent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteTemperatureBufferComponent();
	virtual void BeginPlay() override;

protected:
	float GetCharacterMaxThermalImpedance(const ACharacter* Character) const;
	void UpdateCharacterBufferImpedance(ACharacter* Character);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (ClampMin = "0.0", ClampMax = "1.0",
		ToolTip = "Maps component tags to thermal impedance values. Any primitive component with one of these tags is used as a thermal buffer collider."))
	TMap<FName, float> BufferTagThermalImpedances;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UPrimitiveComponent>, float> ColliderThermalImpedances;

	UPROPERTY(Transient)
	TSet<TObjectPtr<UPrimitiveComponent>> BoundOverlapColliders;
	
	UPROPERTY()
	TObjectPtr<AFretteWorldSettings> WorldSettings;
};