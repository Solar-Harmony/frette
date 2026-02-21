#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "PickUpActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedUp, AActor*, InteractingActor);

UCLASS()
class FRETTE_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APickUpActor();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	UStaticMesh* ItemMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bDestroyOnPickUp = false;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* OverlapSphere;
	
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UFUNCTION()
	void PickedUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	float YawAngle = 0.f;
	
	const float Item_Size = 40.f /*cm*/;
	const float Sphere_Radius_Ratio = 1.5f;
};