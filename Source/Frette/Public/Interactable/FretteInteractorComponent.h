#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable/FretteInteractableInterface.h"
#include "FretteInteractorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UFretteInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFretteInteractorComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateInteractableTarget();
	
	void Interact();

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractTraceRange = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractTraceRadius = 25.f;

	UPROPERTY()
	TScriptInterface<IFretteInteractableInterface> CurrentHoveredActor;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InteractWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> InteractWidgetInstance;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
private:
	UFretteInteractableComponent* GetInteractableComponentFromHover(TScriptInterface<IFretteInteractableInterface> hover) const;
};
