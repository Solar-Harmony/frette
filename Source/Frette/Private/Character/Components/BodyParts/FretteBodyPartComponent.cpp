#include "Character/Components/BodyParts/FretteBodyPartComponent.h"

#include "Character/FretteBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "Character/Player/FrettePlayerCharacter.h"
#include "Character/Components/BodyParts/FretteBodyPartTags.h"
#include "Frette.h"
#include "Net/UnrealNetwork.h"
#include "Character/Player/FrettePlayerController.h"

namespace
{
void BuildRepresentativeBoneMap(const UFretteBonesToTagData* BoneTagDataAsset, TMap<FGameplayTag, FName>& OutMap)
{
	OutMap.Reset();
	if (!IsValid(BoneTagDataAsset))
		return;

	for (const TPair<FName, FGameplayTag>& Entry : BoneTagDataAsset->BoneToBodyPartTag)
	{
		if (!Entry.Value.IsValid() || OutMap.Contains(Entry.Value))
			continue;

		OutMap.Add(Entry.Value, Entry.Key);
	}
}
}

UFretteBodyPartComponent::UFretteBodyPartComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	bReplicateUsingRegisteredSubObjectList = true;
	SetComponentTickInterval(2.0f);
}

void UFretteBodyPartComponent::BeginPlay()
{
	Super::BeginPlay();
	BuildRepresentativeBoneMap(BoneTagDataAsset, BodyPartTagToRepresentativeBoneMap);
	
	// the listen server is also a client, and ReadyForReplication executes after the UI widget Constructs there 
	if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		for (UFretteBodyPartData* Data : BodyPartData)
		{
			UFretteBodyPartInstance* Instance = NewObject<UFretteBodyPartInstance>(this);
			Instance->Initialize(Data, Cast<AFretteBaseCharacter>(GetOwner()));
			BodyPartInstances.Add(Instance);
			BodyPartTagToInstanceMap.Add(Data->BodyPartTag, Instance);

			AddReplicatedSubObject(Instance);
		}
		
		OnBodyPartsInitialized.Broadcast();
	}
}

void UFretteBodyPartComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	BuildRepresentativeBoneMap(BoneTagDataAsset, BodyPartTagToRepresentativeBoneMap);
	
	if (GetOwnerRole() != ROLE_Authority)
		return;
	
	if (!BodyPartInstances.IsEmpty())
		return;	
	
	for (UFretteBodyPartData* Data : BodyPartData)
	{
		UFretteBodyPartInstance* Instance = NewObject<UFretteBodyPartInstance>(this);
		Instance->Initialize(Data, Cast<AFretteBaseCharacter>(GetOwner()));
		BodyPartInstances.Add(Instance);
		BodyPartTagToInstanceMap.Add(Data->BodyPartTag, Instance);

		AddReplicatedSubObject(Instance);
	}
	
	OnBodyPartsInitialized.Broadcast();
}

void UFretteBodyPartComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFretteBodyPartComponent, BodyPartInstances);
}

bool UFretteBodyPartComponent::Ciboire_DeleteMoiPasCalisse() const
{
	const AFrettePlayerCharacter* TabarnakDe = Cast<AFrettePlayerCharacter>(GetOwner());
	if (TabarnakDe == nullptr)
		return false;

	const AFrettePlayerController* Calisse = Cast<AFrettePlayerController>(TabarnakDe->GetController());
	if (Calisse == nullptr)
		return false;

	return Calisse->bFretteCinematicMode;
}

void UFretteBodyPartComponent::AddValueFromBodyPartTag(const FGameplayTag BodyPartTag, const float Value, const FGameplayTag ValueType)
{
	if (GetOwnerRole() != ROLE_Authority)
		return;
	
	if (!BodyPartTag.IsValid())
		return;
	
	if (Ciboire_DeleteMoiPasCalisse())
		return;
	
	if (UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag))
	{
		const FFretteBodyPartContext Result = BodyPart->AddValueByTag(Value, ValueType);
		// FIXME: Not using a proper value delta, also not sure about the way i do it
		const FFretteBodyPartChangeEvent ChangeEvent(BodyPartTag, ValueType, Result.AccumulatedValue, Value);
		Client_NotifyBodyPartChange(ChangeEvent);
		
		UE_LOG(LogFrette, Log, TEXT("Added %f of %s to body part %s"), Value, *ValueType.ToString(), *BodyPartTag.ToString());
	}

}

void UFretteBodyPartComponent::AddValueFromBoneName(const FName BoneName, const float Value, const FGameplayTag ValueType)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		const FGameplayTag BodyPartTag = GetBodyPartFromBoneName(BoneName);
		AddValueFromBodyPartTag(BodyPartTag, Value, ValueType);
	}
}

FGameplayTag UFretteBodyPartComponent::GetBodyPartFromBoneName(const FName BoneName) const
{
	if (!IsValid(BoneTagDataAsset))
		return FGameplayTag();

	if (const FGameplayTag* Tag = BoneTagDataAsset->BoneToBodyPartTag.Find(BoneName))
	{
		return *Tag;
	}
	return FGameplayTag();
}

bool UFretteBodyPartComponent::GetRepresentativeBoneForTag(const FGameplayTag BodyPartTag, FName& OutBoneName) const
{
	if (!BodyPartTag.IsValid())
		return false;

	if (const FName* BoneName = BodyPartTagToRepresentativeBoneMap.Find(BodyPartTag))
	{
		OutBoneName = *BoneName;
		return true;
	}

	if (!IsValid(BoneTagDataAsset))
		return false;

	for (const TPair<FName, FGameplayTag>& Entry : BoneTagDataAsset->BoneToBodyPartTag)
	{
		if (!Entry.Value.MatchesTagExact(BodyPartTag))
			continue;

		OutBoneName = Entry.Key;
		return true;
	}

	return false;
}

UFretteBodyPartInstance* UFretteBodyPartComponent::FindBodyPart(const FGameplayTag BodyPartTag) const
{
	for (TObjectPtr BodyPart : BodyPartInstances)
	{
		if (BodyPart->GetBodyPartTag().MatchesTagExact(BodyPartTag))
			return BodyPart;
	}

	FRETTE_LOG(Error, "Body part with tag %s not found!", BodyPartTag);
	return nullptr;
}

float UFretteBodyPartComponent::GetValueFromBodyPart(FGameplayTag BodyPartTag, FGameplayTag ValueTypeTag) const
{
	UFretteBodyPartInstance* BodyPart = FindBodyPart(BodyPartTag);
	if (BodyPart == nullptr)
		return 0.f;
	
	return BodyPart->FindOrAddAccumulatedValue(ValueTypeTag);
}

void UFretteBodyPartComponent::AddValueToAllParts(float Value, FGameplayTag ValueType)
{
	if (GetOwnerRole() != ROLE_Authority)
		return;
	
	if (Ciboire_DeleteMoiPasCalisse())
		return;
	
	for (UFretteBodyPartInstance* Instance : BodyPartInstances)
	{
		if (Instance)
		{
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

void UFretteBodyPartComponent::ResetValueForAllBodyParts(FGameplayTag ValueType)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		for (UFretteBodyPartInstance* Instance : BodyPartInstances)
		{
			//Reset la valeur a sa valeur de start
			float CurrentValue = Instance->FindOrAddAccumulatedValue(ValueType);
			float StartValue = Instance->GetBodyPartData()->GetStartValueForType(ValueType);
            float Delta = StartValue - CurrentValue;
			AddValueFromBodyPartTag(Instance->GetBodyPartTag(), Delta, ValueType);
		}
	}
}

void UFretteBodyPartComponent::OnRep_BodyPartInstances()
{
	BodyPartTagToInstanceMap.Empty(BodyPartInstances.Num());
	BuildRepresentativeBoneMap(BoneTagDataAsset, BodyPartTagToRepresentativeBoneMap);
	
	for (UFretteBodyPartInstance* Instance : BodyPartInstances)
	{
		BodyPartTagToInstanceMap.Add(Instance->GetBodyPartTag(), Instance);
	}
	
	OnBodyPartsInitialized.Broadcast();
}

void UFretteBodyPartComponent::Client_NotifyBodyPartChange_Implementation(const FFretteBodyPartChangeEvent& ChangeEvent)
{
	OnBodyPartValueChanged.Broadcast(ChangeEvent);
}