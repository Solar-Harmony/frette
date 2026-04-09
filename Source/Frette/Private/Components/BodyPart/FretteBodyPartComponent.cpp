#include "Components/BodyPart/FretteBodyPartComponent.h"

#include "GameplayTagContainer.h"
#include "Components/BodyPart/FretteBodyPartTags.h"
#include "Net/UnrealNetwork.h"

UFretteBodyPartComponent::UFretteBodyPartComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFretteBodyPartComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickInterval(2.0f);

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

float UFretteBodyPartComponent::GetNormalizedCriticalValue(FGameplayTag ValueTag) const
{
	UFretteBodyPartInstance* Head = FindBodyPart(TAG_BodyPart_Head);
	UFretteBodyPartInstance* Torso = FindBodyPart(TAG_BodyPart_Torso);
	
	const float HeadCurrent = Head->FindOrAddAccumulatedValue(ValueTag);
	const float TorsoCurrent = Torso->FindOrAddAccumulatedValue(ValueTag);
	const UFretteBodyPartData* Data = Head->GetSourceData();
	const float HeadMax = Data->GetMaxValueForType(ValueTag);
	const float TorsoMax = Data->GetMaxValueForType(ValueTag);
	
	const float HeadNormalized = HeadCurrent / HeadMax;
	const float TorsoNormalized = TorsoCurrent / TorsoMax;
	
	return FMath::Min(HeadNormalized, TorsoNormalized);
}

void UFretteBodyPartComponent::OnRep_BodyParts() {}