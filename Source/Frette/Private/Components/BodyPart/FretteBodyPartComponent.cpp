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

void UFretteBodyPartComponent::AddValueFromBodyPartTag(const FGameplayTag BodyPartTag, const int Value, const FGameplayTag ValueType)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("Adding %d of %s to body part %s"), Value, *ValueType.ToString(), *BodyPartTag.ToString()));
		if (BodyPartTag.IsValid())
		{
			if (UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag))
			{
				BodyPart->AddValueByTag(Value, ValueType);
			}
		}
	}
}

void UFretteBodyPartComponent::AddValueFromBoneName(const FName BoneName, const int Value, const FGameplayTag ValueType)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		FGameplayTag BodyPartTag = GetBodyPartFromBoneName(BoneName);
		AddValueFromBodyPartTag(BodyPartTag, Value, ValueType);
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

UFretteBodyPartInstance* UFretteBodyPartComponent::FindBodyPart(const FGameplayTag BodyPartTag)
{
	for (TObjectPtr BodyPart : BodyPartInstances)
	{
		if (BodyPart->GetBodyPartTag().MatchesTagExact(BodyPartTag))
			return BodyPart;
	}

	UE_LOG(LogTemp, Error, TEXT("Body part with tag %s not found!"), *BodyPartTag.ToString());

	return nullptr;
}

void UFretteBodyPartComponent::AddValueToAllParts(int Value, FGameplayTag ValueType)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		for (UFretteBodyPartInstance* Instance : BodyPartInstances)
		{
			if (Instance)
				Instance->AddValueByTag(Value, ValueType);
		}
	}
}

void UFretteBodyPartComponent::OnRep_BodyParts() {}