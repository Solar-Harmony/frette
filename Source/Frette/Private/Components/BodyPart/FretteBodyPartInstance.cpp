#include "Components/BodyPart/FretteBodyPartInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

void UFretteBodyPartInstance::Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner)
{
	SourceData = InSourceData;
	CurrentHealth = SourceData->MaxHealth;
	OwnerCharacter = Owner;
}

void UFretteBodyPartInstance::ApplyDamage(const float Damage, const FGameplayTag DamageType)
{
	CurrentHealth -= Damage;
	AccumulatedDamageByType.FindOrAdd(DamageType) += Damage;

	for (auto Rule : SourceData->EffectRules)
	{
		if (IsTriggered(Rule, Damage, DamageType))
		{
			Rule.bHasTriggered = true;
			ApplyEffect(Rule);
		}
	}
}

//TODO: Ajouter le soin de parties du corps, retirer les éffets qui on été ajouté et reset les infos (hasTriggered, accumulatedDamageByType)

bool UFretteBodyPartInstance::IsTriggered(const FBodyPartEffectRule& Rule, const float Damage, const FGameplayTag DamageType) const
{
	if (Rule.bHasTriggered)
		return false;

	if (DamageType == Rule.DamageType
		&& (Damage >= Rule.InstantDamageThreshold || AccumulatedDamageByType.FindRef(DamageType) >= Rule.CumulativeDamageThreshold)
		|| Rule.TriggerOnDeath && CurrentHealth <= 0.f)
		return true;

	return false;
}

void UFretteBodyPartInstance::ApplyEffect(const FBodyPartEffectRule& Rule) const
{
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle NewHandle = OwnerASC->MakeOutgoingSpec(Rule.GameplayEffect, 1, EffectContext);

	OwnerASC->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Applied effect %s to body part %s"), *Rule.GameplayEffect->GetName(), *SourceData->BodyPartTag.ToString()));
}

void UFretteBodyPartInstance::OnRep_CurrentHealth()
{
	UE_LOG(LogTemp, Log, TEXT("MyValue changed to %f"), CurrentHealth);
}

void UFretteBodyPartInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFretteBodyPartInstance, CurrentHealth);
}