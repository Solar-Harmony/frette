#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable/FretteInteractableInterface.h"
#include "FretteInteractorComponent.generated.h"

UCLASS(ClassGroup=(Frette), meta=(BlueprintSpawnableComponent))
class UFretteInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFretteInteractorComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateInteractableTarget();
	void Interact();

	UPROPERTY(EditDefaultsOnly, Category="Frette")
	float InteractTraceRange = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Frette")
	float InteractTraceRadius = 25.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TSubclassOf<UUserWidget> InteractWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> InteractWidgetInstance;
	
	UPROPERTY()
	TScriptInterface<IFretteInteractableInterface> CurrentHoveredActor;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
private:
	UFretteInteractableComponent* GetInteractableComponentFromHover(TScriptInterface<IFretteInteractableInterface> hover) const;
};
