#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FretteTemperatureBufferComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteTemperatureBufferComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TObjectPtr<UShapeComponent> OverlapShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (ClampMin = "0.0", ClampMax = "1.0",
		ToolTip = "This is a number betwwen 0 and 1 for which 0 means no effect on temperature flows and 1 means the flows are completely cancelled."))
	float ThermalImpedance = 0.5;
	
	UPROPERTY()
	TSet<TObjectPtr<ACharacter>> OverlappingCharacters;
	
	FGuid UniqueId;
};