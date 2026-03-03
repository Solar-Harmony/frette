#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FretteInteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginHover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndHover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRETTE_API UFretteInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFretteInteractableComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable)
	FOnBeginHover OnBeginHover;
	
	UPROPERTY(BlueprintAssignable)
	FOnEndHover OnEndHover;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText Message = FText::FromString("Press E to interact");
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float OutlineThickness = 2.f;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float OutlineAlpha = 0.7f;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FColor OutlineColor = FColor::White;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bShowOutline = false;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bShowMessage = true;
};
