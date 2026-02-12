#pragma once

#include "CoreMinimal.h"
#include "FrettePlayerState.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Input/FretteInputConfig.h"
#include "FrettePlayerController.generated.h"

class UCameraComponent;

UCLASS()
class FRETTE_API AFrettePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Create Widgets and Viewmodels"))
	void SetupWidgetsAndViewModels();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TObjectPtr<class UInputMappingContext> DefaultInputContext;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Input")
	TObjectPtr<UFretteInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> FretteAbilitySystemComponent;

	UFUNCTION(BlueprintPure, Category = "Frette")
	AFrettePlayerState* GetFrettePlayerState() const { return GetPlayerState<AFrettePlayerState>(); }

	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	UFretteAbilitySystemComponent* GetASC();
};