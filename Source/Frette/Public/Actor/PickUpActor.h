#pragma once

#include "CoreMinimal.h"
#include "PostProcessSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractibleInterface.h"
#include "PickUpActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedUp, AActor*, InteractingActor);

UCLASS()
class FRETTE_API APickUpActor : public AActor, public IInteractibleInterface
{
	GENERATED_BODY()
	
public:	
	APickUpActor();
	
	virtual void BeginPlay() override;
	
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
	
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;
	
	UPROPERTY()
	UPostProcessSubsystem* PostProcess;
	
	UPROPERTY()
	UTextRenderComponent* InteractibleText;
	
	virtual void BeginHover() override;
	virtual void EndHover() override;
	virtual void Interact() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	float YawAngle = 0.f;
	
	const float Item_Size = 40.f /*cm*/;
	const float Sphere_Radius_Ratio = 1.5f;
};