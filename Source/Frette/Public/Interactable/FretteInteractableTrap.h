#pragma once

#include "CoreMinimal.h"
#include "FretteInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "FretteInteractableTrap.generated.h"

UCLASS()
class FRETTE_API AFretteInteractableTrap : public AActor, public IFretteInteractableInterface
{
	GENERATED_BODY()

public:
	AFretteInteractableTrap();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TObjectPtr<UFretteInteractableComponent> Interactable;

	virtual UFretteInteractableComponent* GetInteractableComponent() override { return Interactable; }
};