#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "FretteBodyPartTags.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "FretteBodyPartAttributeSet.generated.h"

UCLASS()
class FRETTE_API UFretteBodyPartAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet, Health);
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UFretteBodyPartAttributeSet, MaxHealth);
	
	virtual FGameplayTag GetDamageTag() const PURE_VIRTUAL(GetDamageTag, return FGameplayTag(););

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME_CONDITION_NOTIFY(UFretteBodyPartAttributeSet, Health, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UFretteBodyPartAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	}
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteBodyPartAttributeSet, Health, OldHealth);
	}
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFretteBodyPartAttributeSet, MaxHealth, OldMaxHealth);
	}
};
