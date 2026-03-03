#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Input/FretteInputConfig.h"
#include "InputAction.h"
#include "Components/FretteInteractorComponent.h"
#include "FrettePlayerController.generated.h"

class UCameraComponent;

UCLASS()
class FRETTE_API AFrettePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

private:
	AFrettePlayerController();
	
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

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFretteInteractorComponent> Interactor;

	void OnInteractPressed();
};