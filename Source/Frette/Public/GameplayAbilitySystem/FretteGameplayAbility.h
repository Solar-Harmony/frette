

#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FretteGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FRETTE_API UFretteGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFretteGameplayAbility();
	
	//Les GameplayAbilities on un InputPressed et InputRelease mais ça fonctionne seulement pour l'ancien input system
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Input")
	void OnInputReleased();
	virtual void OnInputReleased_Implementation();
	
	UFUNCTION(BlueprintPure, Category = "Ability")
	FGameplayAbilitySpecHandle GetCurrentAbilityHandle() const { return CurrentSpecHandle; }
	
	UFUNCTION(BlueprintPure, Category = "Ability")
	AActor* GetAvatarActor() const { return CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr; }
};
