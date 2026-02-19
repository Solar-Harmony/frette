#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Input/FretteInputConfig.h"
#include "Interface/InteractibleInterface.h"
#include "FrettePlayerController.generated.h"

class UCameraComponent;

UCLASS()
class FRETTE_API AFrettePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TObjectPtr<class UInputMappingContext> DefaultInputContext;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Frette|Input")
	TObjectPtr<UFretteInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> FretteAbilitySystemComponent;

	UFretteAbilitySystemComponent* GetASC();

	void UpdateInteractableTarget();

	UPROPERTY(EditDefaultsOnly, Category="Frette|Interaction")
	float InteractTraceRange = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Interaction")
	float InteractTraceRadius = 25.f;

	UPROPERTY()
	TScriptInterface<IInteractibleInterface> CurrentHoveredActor;
};