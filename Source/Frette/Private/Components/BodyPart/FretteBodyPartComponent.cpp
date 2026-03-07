#include "Components/BodyPart/FretteBodyPartComponent.h"

#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

UFretteBodyPartComponent::UFretteBodyPartComponent() {}

void UFretteBodyPartComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		BodyParts = DefaultBodyParts;

		for (FBodyPartHealthData& BodyPart : BodyParts)
		{
			BodyPart.CurrentHealth = BodyPart.MaxHealth;
		}
	}
}

void UFretteBodyPartComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFretteBodyPartComponent, BodyParts);

}

void UFretteBodyPartComponent::ServerApplyDamage_Implementation(FGameplayTag BodyPartTag, float Damage)
{
	if (FBodyPartHealthData* BodyPart = FindBodyPart(BodyPartTag))
	{
		BodyPart->CurrentHealth = FMath::Max(0.f, BodyPart->CurrentHealth - Damage);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
			FString::Printf(TEXT("received damage for body part %s: %.1f"), *BodyPartTag.ToString(), Damage));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
			FString::Printf(TEXT("%s health is now %.1f"), *BodyPartTag.ToString(), BodyPart->CurrentHealth));
	}
}

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
	const FBodyPartHealthData* BodyPart = FindBodyPart(BodyPartTag);
	return BodyPart ? BodyPart->CurrentHealth : 0.f;
}

FBodyPartHealthData* UFretteBodyPartComponent::FindBodyPart(const FGameplayTag BodyPartTag)
{
	for (FBodyPartHealthData& BodyPart : BodyParts)
	{
		if (BodyPart.BodyPartTag.MatchesTagExact(BodyPartTag))
			return &BodyPart;
	}

	UE_LOG(LogTemp, Error, TEXT("Body part with tag %s not found!"), *BodyPartTag.ToString());

	return nullptr;
}

void UFretteBodyPartComponent::OnRep_BodyParts() {}