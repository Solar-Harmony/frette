#include "Components/FretteTemperatureComponent.h"

#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Frette/Frette.h"

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
		TMap<FGameplayTag, float> BoneTempAccum;

		for (const auto& Entry : BodyPartTemperatureContributions)
		{
			const FTemperatureKey& Key = Entry.Key;
			const FTemperatureContribution Contributionn = Entry.Value;

			const FGameplayTag BoneTag = Key.BodyPart;

			float& AccumulatedFlow = BoneFlowAccum.FindOrAdd(BoneTag);
			float& AccumulatedTemp = BoneTempAccum.FindOrAdd(BoneTag);
			AccumulatedFlow += Contributionn.Flow;
			AccumulatedTemp += Contributionn.Weight * Contributionn.Temperature;
		}

		for (const auto& Pair : BoneFlowAccum)
		{
			const FGameplayTag& BoneTag = Pair.Key;
			float NetFlow = Pair.Value;
			float NetTemperature = BoneTempAccum.FindOrAdd(BoneTag);
			
			// Truncate the net temp just to be safe
			NetTemperature = FMath::Clamp(NetTemperature, MinTemperature, MaxTemperature);

			float CurrentTemp =
				BodyPartComponent->GetValueFromBodyPart(BoneTag, TemperatureEffectTag);
			
			if (CurrentTemp < MinTemperature || CurrentTemp > MaxTemperature)
			{
				UE_LOG(LogFrette, Log, TEXT("Your temperature system is not doing good in terms of numerical stability no cap!! Truncating temp biatch!!!"));
				continue;
			}

			const float AmbientFlow = DiffusionSpeed * (AmbientTemperature - CurrentTemp);
			NetFlow += AmbientFlow;
			
			// Make things more physically accurate??
			const float DeltaTime = GetWorld()->GetDeltaSeconds();
			NetFlow *= DeltaTime;
			
			// DAMPING We will make the behavior asymptotic near the target temp to make the system equilibrate
			const float Influence = (NetTemperature - CurrentTemp) / (MaxTemperature - MinTemperature);
			NetFlow *= FMath::Clamp(Influence, Damping, 1.0);
			
			BodyPartComponent->AddValueFromBodyPartTag(
				BoneTag,
				NetFlow,
				TemperatureEffectTag
			);
		}
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureContribution(const FTemperatureContribution Contribution, const FGameplayTag BodyPartTag, FGuid SourceId)
{
	if (FTemperatureContribution* CurrentValue = BodyPartTemperatureContributions.Find(FTemperatureKey(BodyPartTag, SourceId)))
	{
		*CurrentValue = Contribution;
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureContribution(const FTemperatureContribution Contribution, const FName BoneName, FGuid SourceId)
{
	FGameplayTag BodyPartTag = BodyPartComponent->GetBodyPartFromBoneName(BoneName);

	AddBodyPartTemperatureContribution(Contribution, BodyPartTag, SourceId);
}

void UFretteTemperatureComponent::AddToAmbientTemperature(const float NewAmbientTemperature)
{
	AmbientTemperature += NewAmbientTemperature;
}

void UFretteTemperatureComponent::ClearBodyPartTemperatureContribution(FGameplayTag BodyPartTag, FGuid SourceId)
{
	BodyPartTemperatureContributions.Remove(FTemperatureKey(BodyPartTag, SourceId));
}

void UFretteTemperatureComponent::ClearBodyPartTemperatureContribution(FName BoneName, FGuid SourceId)
{
	FGameplayTag BodyPartTag = BodyPartComponent->GetBodyPartFromBoneName(BoneName);
	
	BodyPartTemperatureContributions.Remove(FTemperatureKey(BodyPartTag, SourceId));
}

void UFretteTemperatureComponent::ClearBodyPartTemperatureContributions(FGuid SourceId)
{
	for (auto It = BodyPartTemperatureContributions.CreateIterator(); It; ++It)
	{
		if (It->Key.SourceId == SourceId)
			It.RemoveCurrent();
	}
}
