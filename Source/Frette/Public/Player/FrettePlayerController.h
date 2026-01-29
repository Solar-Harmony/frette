

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
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UFretteInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> AuraAbilitySystemComponent;

	UFretteAbilitySystemComponent* GetASC();
};


