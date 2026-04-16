#include "Components/FretteTemperatureComponent.h"

#include "Components/BodyPart/FretteBodyPartComponent.h"

UFretteTemperatureComponent::UFretteTemperatureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFretteTemperatureComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickInterval(TimeBetweenTemperatureChange);

	BodyPartComponent = GetOwner()->GetComponentByClass<UFretteBodyPartComponent>();
}

void UFretteTemperatureComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OnTemperatureTick();
}

void UFretteTemperatureComponent::OnTemperatureTick()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (!BodyPartComponent)
			return;
		
		TMap<FGameplayTag, float> BoneFlowAccum;

		for (const auto& Entry : BodyPartTemperatureFlows)
		{
			const FTemperatureKey& Key = Entry.Key;
			const float FlowValue = Entry.Value;

			const FGameplayTag BoneTag = Key.BodyPart;

			float& AccumulatedFlow = BoneFlowAccum.FindOrAdd(BoneTag);
			AccumulatedFlow += FlowValue;
		}

		for (const auto& Pair : BoneFlowAccum)
		{
			const FGameplayTag& BoneTag = Pair.Key;
			float NetFlow = Pair.Value;

			float CurrentTemp =
				BodyPartComponent->GetValueFromBodyPart(BoneTag, TemperatureEffectTag);
			
			if (CurrentTemp < MinTemperature || CurrentTemp > MaxTemperature)
				continue;

			const float AmbientDelta =
				DiffusionSpeed * (AmbientTemperature - CurrentTemp);

			NetFlow += AmbientDelta;
			
			BodyPartComponent->AddValueFromBodyPartTag(
				BoneTag,
				NetFlow,
				TemperatureEffectTag
			);
		}
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureFlow(const float NewTargetTemperature, const FGameplayTag BodyPartTag, FGuid SourceId)
{
	if (float* CurrentValue = BodyPartTemperatureFlows.Find(FTemperatureKey(BodyPartTag, SourceId)))
	{
		*CurrentValue += NewTargetTemperature;
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureFlow(const float NewTargetTemperature, const FName BoneName, FGuid SourceId)
{
	FGameplayTag BodyPartTag = BodyPartComponent->GetBodyPartFromBoneName(BoneName);

	AddBodyPartTemperatureFlow(NewTargetTemperature, BodyPartTag, SourceId);
}

void UFretteTemperatureComponent::AddToAmbientTemperature(const float NewAmbientTemperature)
{
	AmbientTemperature += NewAmbientTemperature;
}

void UFretteTemperatureComponent::ClearBodyPartTemperatureFlow(FGameplayTag BodyPartTag, FGuid SourceId)
{
	BodyPartTemperatureFlows.Remove(FTemperatureKey(BodyPartTag, SourceId));
}

void UFretteTemperatureComponent::ClearBodyPartTemperatureFlow(FName BoneName, FGuid SourceId)
{
	FGameplayTag BodyPartTag = BodyPartComponent->GetBodyPartFromBoneName(BoneName);
	
	BodyPartTemperatureFlows.Remove(FTemperatureKey(BodyPartTag, SourceId));
}

void UFretteTemperatureComponent::ClearBodyPartTemperatureFlows(FGuid SourceId)
{
	for (auto It = BodyPartTemperatureFlows.CreateIterator(); It; ++It)
	{
		if (It->Key.SourceId == SourceId)
			It.RemoveCurrent();
	}
}
