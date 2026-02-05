#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Equipments/FretteEquipmentDataAsset.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Weapons/FretteRangedWeaponInstance.h"
#include "FretteBaseCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class FRETTE_API AFretteBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	void ApplyStartupEffects() const;
	void ApplyDefaultAttributeEffect(const FGameplayEffectContextHandle& EffectContext) const;
	void ApplyDefaultStartupEffect(const FGameplayEffectContextHandle& EffectContext) const;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "Frette|AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditAnywhere, Category = "Frette|AbilitySystem")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Archetype")
	TObjectPtr<UAbilitySetDataAsset> ArchetypeLoadout;

	UPROPERTY(EditAnywhere)
	int CharacterLevel = 1;

	virtual void InitAbilityActorInfo();

	void SubToAttributeChanges();

	void OnMaxSpeedChanged(const FOnAttributeChangeData& Data) const;

	virtual void BeginPlay() override;
	//For testing purposes
	//@TODO:Faudrait le changer pour faire un vrai equiment manager system ou wtv
	UPROPERTY(EditAnywhere, Category="Frette|Equipment")
	TObjectPtr<UFretteEquipmentDataAsset> EquippedItem;

	UPROPERTY()
	TObjectPtr<UFretteWeaponInstance> CurrentWeaponInstance = nullptr;

};