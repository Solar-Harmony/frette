#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FretteTemperatureZoneComponent.generated.h"

//Fonctionne pas si je met just le TSet<FName> Bones; dans le TMap for some reason
USTRUCT()
struct FCharacterActiveBones
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FName> Bones;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureZoneComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteTemperatureZoneComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TemperatureZoneTick();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	TArray<FName> GetAffectedBonesFromSkelMesh(const USkeletalMeshComponent* SkelMesh) const;

	UPROPERTY()
	TObjectPtr<UShapeComponent> OverlapShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (Units = "Celsius"))
	float Temperature = -30;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frette|Temperature", meta = (ForceUnits = "°C/s"))
	float Flow = -12;

	UPROPERTY()
	TMap<TObjectPtr<ACharacter>, FCharacterActiveBones> ActiveBonesPerCharacter;

	UPROPERTY()
	TSet<TObjectPtr<ACharacter>> OverlappingCharacters;
	
	FGuid UniqueId;
};