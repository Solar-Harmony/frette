#pragma once

#include "CoreMinimal.h"
#include "FrettePostProcessSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/FretteInteractableInterface.h"
#include "PickUpActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedUp, AActor*, InteractingActor);

UCLASS()
class FRETTE_API APickUpActor : public AActor, public IFretteInteractableInterface
{
	GENERATED_BODY()
	
public:	
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
	USphereComponent* OverlapSphere;
	
	UPROPERTY()
	UFretteInteractableComponent* Interactable;
	
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;
	
	UFUNCTION()
	void Interact();
	
	virtual UFretteInteractableComponent* GetInteractableComponent() override { return Interactable; }
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	float YawAngle = 0.f;
	
	const float Item_Size = 40.f /*cm*/;
	const float Sphere_Radius_Ratio = 1.5f;
};