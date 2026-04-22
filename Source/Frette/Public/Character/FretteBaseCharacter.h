#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/FallDamageComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
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

public:
	bool GetIsDead() const { return bIsDead; }

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFretteBodyPartComponent> BodyPartComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFretteTemperatureComponent> BodyTemperatureComponent;

	UFUNCTION(BlueprintCallable)
	virtual void Die();
	
	UFUNCTION(BlueprintCallable)
	virtual void Revive();

protected:
	AFretteBaseCharacter();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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

	UPROPERTY()
	bool bIsDead = false;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	void ApplyStartupEffects();
	void ApplyDefaultAttributeEffect(const FGameplayEffectContextHandle& EffectContext) const;
	void ApplyDefaultStartupEffect(const FGameplayEffectContextHandle& EffectContext) const;
	virtual void InitAbilityActorInfo();
	void SubToAttributeChanges();
	void OnMaxSpeedChanged(const FOnAttributeChangeData& Data) const;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_HandleDeath(FVector DeathVelocity);

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_HandleRevive();
};