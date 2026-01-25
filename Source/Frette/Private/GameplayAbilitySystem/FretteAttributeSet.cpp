


#include "GameplayAbilitySystem/FretteAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"

#include "Net/UnrealNetwork.h"

UFretteAttributeSet::UFretteAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitStamina(50.f);
	InitMaxStamina(50.f);
}

void UFretteAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Enregistre des variables pour la réplication
	//Le REPNOTIFY_Always fait que  même si la valeur est modifier a la même valeur, la fonction de notification sera appelé
	DOREPLIFETIME_CONDITION_NOTIFY(UFretteAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UFretteAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UFretteAttributeSet, Stamina, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UFretteAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always)
	
}


void UFretteAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
}

void UFretteAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

//Annonce au gameplaySystem que l'attribut a été mis à jour
void UFretteAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteAttributeSet, Health, OldHealth);
}

void UFretteAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteAttributeSet, MaxHealth, OldMaxHealth);
}

void UFretteAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteAttributeSet, Stamina, OldMana);
}

void UFretteAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteAttributeSet, MaxStamina, OldMaxMana);
}
