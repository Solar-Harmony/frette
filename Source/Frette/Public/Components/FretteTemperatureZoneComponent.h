#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/FretteWorldSettings.h"
#include "FretteTemperatureZoneComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureZoneComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteTemperatureZoneComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TObjectPtr<UShapeComponent> OverlapShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float Temperature = -30;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (ForceUnggits = "°C/s"))
	float Flow = -12;

	UPROPERTY()
	TSet<TObjectPtr<ACharacter>> OverlappingCharacters;
	
	UPROPERTY()
	TObjectPtr<AFretteWorldSettings> WorldSettings;
	
	FGuid UniqueId;
};