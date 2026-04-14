#include "Components/BodyPart/FretteBodyPartComponent.h"

#include "GameplayTagContainer.h"
#include "Components/BodyPart/FretteBodyPartTags.h"
#include "Frette/Frette.h"
#include "Net/UnrealNetwork.h"

UFretteBodyPartComponent::UFretteBodyPartComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	bReplicateUsingRegisteredSubObjectList = true;
	SetComponentTickInterval(2.0f);
}

void UFretteBodyPartComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	
	if (GetOwnerRole() != ROLE_Authority)
		return;
	
	for (UFretteBodyPartData* Data : BodyPartData)
	{
		UFretteBodyPartInstance* Instance = NewObject<UFretteBodyPartInstance>(this);
		Instance->Initialize(Data, Cast<AFretteBaseCharacter>(GetOwner()));
		BodyPartInstances.Add(Instance);
		BodyPartTagToInstanceMap.Add(Data->BodyPartTag, Instance);

		AddReplicatedSubObject(Instance);
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
		UE_LOG(LogFrette, Log, TEXT("Adding %d of %s to body part %s"), Value, *ValueType.ToString(), *BodyPartTag.ToString());
		
		if (BodyPartTag.IsValid())
		{
			if (UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag))
			{
				const FFretteBodyPartContext Result = BodyPart->AddValueByTag(Value, ValueType);
				// FIXME: Not using a proper value delta, also not sure about the way i do it
				const FFretteBodyPartChangeEvent ChangeEvent(BodyPartTag, ValueType, Result.AccumulatedValue, Value);
				Client_NotifyBodyPartChange(ChangeEvent);
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

UFretteBodyPartInstance* UFretteBodyPartComponent::FindBodyPart(const FGameplayTag BodyPartTag) const
{
	for (TObjectPtr BodyPart : BodyPartInstances)
	{
		if (BodyPart->GetBodyPartTag().MatchesTagExact(BodyPartTag))
			return BodyPart;
	}

	UE_LOG(LogTemp, Error, TEXT("Body part with tag %s not found!"), *BodyPartTag.ToString());

	return nullptr;
}

void UFretteBodyPartComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int UFretteBodyPartComponent::GetValueFromBodyPart(FGameplayTag BodyPartTag, FGameplayTag ValueType) const
{
	return FindBodyPart(BodyPartTag)->FindOrAddAccumulatedValue(ValueType);
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

// TMap cannot be replicated so we make sure to populate it on clients too
UFretteBodyPartInstance* UFretteBodyPartComponent::GetInstanceFromBodyPartTag(FGameplayTag BodyPartTag)
{
	TObjectPtr<UFretteBodyPartInstance>* Instance = BodyPartTagToInstanceMap.Find(BodyPartTag);
	if (Instance == nullptr)
	{
		FRETTE_LOG(Error, "Body part with tag %s not found!", *BodyPartTag.ToString());
		return nullptr;
	}
	
	return *Instance;
}

float UFretteBodyPartComponent::GetNormalizedCriticalValue(FGameplayTag ValueTag, bool bForFeedback) const
{
	static TArray<float> NormalizedValues;
	NormalizedValues.Reset(BodyPartInstances.Num());
	
	for (UFretteBodyPartInstance* Instance : BodyPartInstances)
	{
		const UFretteBodyPartData* Data = Instance->GetBodyPartData();
		const FFretteBodyPartValueTypeConfig* Config = Data->GetValueTypeConfig(ValueTag);
		if (Config == nullptr)
		{
			FRETTE_LOG(Error, "T cave");
			continue;
		}
		
		if (!Config->bIsCritical)
			continue;
		
		const float Value = Instance->FindOrAddAccumulatedValue(ValueTag);
		const float Min = bForFeedback ? Config->FeedbackLowValue : Config->MinValue;
		const float Max = bForFeedback? Config->FeedbackHighValue : Config->MaxValue;
		const float Normalized = FMath::Clamp((Value - Min) / (Max - Min), 0.0f, 1.0f);
		NormalizedValues.Add(Normalized);
	}
	
	return FMath::Min(NormalizedValues);
}

void UFretteBodyPartComponent::OnRep_BodyPartInstances()
{
	BodyPartTagToInstanceMap.Empty(BodyPartInstances.Num());
	
	for (UFretteBodyPartInstance* Instance : BodyPartInstances)
	{
		BodyPartTagToInstanceMap.Add(Instance->GetBodyPartTag(), Instance);
	}
}

void UFretteBodyPartComponent::Client_NotifyBodyPartChange_Implementation(const FFretteBodyPartChangeEvent& ChangeEvent)
{
	OnBodyPartValueChanged.Broadcast(ChangeEvent);
}