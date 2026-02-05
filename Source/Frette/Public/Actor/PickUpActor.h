

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
	// Sets default values for this actor's properties
	APickUpActor();
	
	UFUNCTION()
	void PickedUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					   bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	UStaticMesh* ItemMesh;
	
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bDestroyOnPickUp = false;
	
	UPROPERTY()
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY()
	USceneComponent* Root;
	
	UPROPERTY()
	USphereComponent* Sphere;
	
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	
	float YawAngle = 0.f;
	float ZPhase = 0.f;
	
	const float Item_Size = 40.f /*cm*/;
	const float Item_Rotation_Speed = (360.f /*degrees*/ / 4.f /*seconds*/);
	const float Item_Z_Mod_Speed = (2.f * PI /*radians*/ / 6.f /*seconds*/);
	const float Item_Z_Mod_Range = 10.f /*cm*/;
	const float Item_Z_Offset = 5.f /*cm*/;
};
