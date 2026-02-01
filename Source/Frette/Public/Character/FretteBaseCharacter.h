

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "FretteBaseCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class FRETTE_API AFretteBaseCharacter : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFretteBaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void ApplyStartupEffects() const;
	void ApplyDefaultAttributeEffect(const FGameplayEffectContextHandle& EffectContext) const;
	void ApplyDefaultStartupEffect(const FGameplayEffectContextHandle& EffectContext) const;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, Category = "Frette|AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultAttributes;
	
	UPROPERTY(EditAnywhere, Category = "Frette|AbilitySystem")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Archetype")
	TObjectPtr<UDA_ArchetypeLoadout> ArchetypeLoadout;
	
	UPROPERTY(EditAnywhere)
	int CharacterLevel = 1;
	
	virtual void InitAbilityActorInfo();
	
	void SubToAttributeChanges();
	
	void OnMaxSpeedChanged(const FOnAttributeChangeData& Data) const;
};
