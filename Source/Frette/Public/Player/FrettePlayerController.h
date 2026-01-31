

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Input/FretteInputConfig.h"
#include "FrettePlayerController.generated.h"


class UCameraComponent;
/**
 * 
 */
UCLASS()
class FRETTE_API AFrettePlayerController : public APlayerController
{
	GENERATED_BODY()
	

protected:
	virtual void SetupInputComponent() override;

private:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TObjectPtr<class UInputMappingContext> DefaultInputContext;
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Input")
	TObjectPtr<UFretteInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> AuraAbilitySystemComponent;

	UFretteAbilitySystemComponent* GetASC();
};


