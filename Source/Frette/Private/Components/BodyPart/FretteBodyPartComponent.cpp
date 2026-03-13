#include "Components/BodyPart/FretteBodyPartComponent.h"

#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

UFretteBodyPartComponent::UFretteBodyPartComponent() {}

void UFretteBodyPartComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		for (const auto& Data : BodyPartData)
		{
			UFretteBodyPartInstance* Instance = NewObject<UFretteBodyPartInstance>(this);
			Instance->Initialize(Data, Cast<AFretteBaseCharacter>(GetOwner()));
			BodyPartInstances.Add(Instance);
			AddReplicatedSubObject(Instance);
		}
	}
}

void UFretteBodyPartComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFretteBodyPartComponent, BodyPartInstances);
}

void UFretteBodyPartComponent::ServerApplyDamage_Implementation(FGameplayTag BodyPartTag, float Damage)
{
	if (UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag))
	{
		BodyPart->ApplyDamage(Damage);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
			FString::Printf(TEXT("received damage for body part %s: %.1f"), *BodyPartTag.ToString(), Damage));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
			FString::Printf(TEXT("%s health is now %.1f"), *BodyPartTag.ToString(), BodyPart->CurrentHealth));
	}
}

//TODO: enlever les doublons de méthodes
void UFretteBodyPartComponent::ApplyDamageFromHit(const FName BoneName, float Damage)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		FGameplayTag BodyPartTag = GetBodyPartFromBoneName(BoneName);
		if (BodyPartTag.IsValid())
		{
			ServerApplyDamage(BodyPartTag, Damage);
		}
	}
}

void UFretteBodyPartComponent::ApplyDamageFromHit(const FGameplayTag BodyPartTag, float Damage)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		if (BodyPartTag.IsValid())
		{
			ServerApplyDamage(BodyPartTag, Damage);
		}
	}
}

void UFretteBodyPartComponent::AddStatusEffectStackFromHit(const FGameplayTag BodyPartTag, int StackAmount, FGameplayTag EffectTag)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		if (BodyPartTag.IsValid())
		{
			if (UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag))
			{
				BodyPart->AddStatusEffectStack(StackAmount, EffectTag);
			}
		}
	}
}

FGameplayTag UFretteBodyPartComponent::GetBodyPartFromBoneName(const FName BoneName) const
{
	if (const FGameplayTag* Tag = BoneTagDataAsset.Get()->BoneToBodyPartTag.Find(BoneName))
	{
		return *Tag;
	}
	return FGameplayTag();
}

float UFretteBodyPartComponent::GetBodyPartHealth(const FGameplayTag BodyPartTag)
{
	const UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag);
	return BodyPart ? BodyPart->CurrentHealth : 0.f;
}

UFretteBodyPartInstance* UFretteBodyPartComponent::FindBodyPart(const FGameplayTag BodyPartTag)
{
	for (TObjectPtr BodyPart : BodyPartInstances)
	{
		if (BodyPart->GetAssociatedTag().MatchesTagExact(BodyPartTag))
			return BodyPart;
	}

	UE_LOG(LogTemp, Error, TEXT("Body part with tag %s not found!"), *BodyPartTag.ToString());

	return nullptr;
}

void UFretteBodyPartComponent::AddStatusEffectStackToAllParts(int StackAmount, FGameplayTag EffectTag)
{
	for (UFretteBodyPartInstance* Instance : BodyPartInstances)
	{
		if (Instance)
			Instance->AddStatusEffectStack(StackAmount, EffectTag);
	}
}

void UFretteBodyPartComponent::OnRep_BodyParts() {}