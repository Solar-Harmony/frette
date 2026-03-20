#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/FallDamageComponent.h"
#include "Equipment/FretteEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "FretteBaseCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class FRETTE_API AFretteBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	AFretteBaseCharacter();

	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "Frette|AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditAnywhere, Category = "Frette|AbilitySystem")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(EditDefaultsOnly, Category= "Frette|Archetype")
	TObjectPtr<UFretteAbilitySetDataAsset> ArchetypeLoadout;

	UPROPERTY(EditDefaultsOnly, Category= "Frette")
	TObjectPtr<UFallDamageComponent> FallDamageComponent;

	UPROPERTY(EditAnywhere)
	int CharacterLevel = 1;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	void ApplyStartupEffects();
	void ApplyDefaultAttributeEffect(const FGameplayEffectContextHandle& EffectContext) const;
	void ApplyDefaultStartupEffect(const FGameplayEffectContextHandle& EffectContext) const;
	virtual void InitAbilityActorInfo();
	void SubToAttributeChanges();
	void OnMaxSpeedChanged(const FOnAttributeChangeData& Data) const;
};