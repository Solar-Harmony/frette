#include "Components/FretteTemperatureComponent.h"

#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Frette/Frette.h"

UFretteTemperatureComponent::UFretteTemperatureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	FGameplayTag Feet = FGameplayTag::RequestGameplayTag(FName("Frette.BodyPart.Feet"));
	FGameplayTag Legs = FGameplayTag::RequestGameplayTag(FName("Frette.BodyPart.Legs"));
	FGameplayTag Hands = FGameplayTag::RequestGameplayTag(FName("Frette.BodyPart.Hands"));
	FGameplayTag Torso = FGameplayTag::RequestGameplayTag(FName("Frette.BodyPart.Torso"));
	FGameplayTag Head = FGameplayTag::RequestGameplayTag(FName("Frette.BodyPart.Head"));

	BoneTagNeighbours.Add(Head, FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{ Torso }));
	BoneTagNeighbours.Add(Torso, FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{ Head, Hands, Legs }));
	BoneTagNeighbours.Add(Hands, FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{ Torso }));
	BoneTagNeighbours.Add(Legs, FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{ Torso, Feet }));
	BoneTagNeighbours.Add(Feet, FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{ Legs }));
}

void UFretteTemperatureComponent::OnRegister()
{
	Super::OnRegister();

	if (UWorld* World = GetWorld())
		WorldSettings = Cast<AFretteWorldSettings>(World->GetWorldSettings());
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
	if (!IsValid(WorldSettings))
		return;

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (!BodyPartComponent)
			return;

		TMap<FGameplayTag, float> BoneFlowAccum;
		TMap<FGameplayTag, float> BoneTempAccum;

		for (const TObjectPtr<UFretteBodyPartData>& Data : BodyPartComponent->BodyPartData)
		{
			if (Data->BodyPartTag.IsValid())
			{
				BoneFlowAccum.Add(Data->BodyPartTag, 0.0f);
				BoneTempAccum.Add(Data->BodyPartTag, 0.0f);
			}
		}

		for (const auto& Entry : BodyPartTemperatureContributions)
		{
			const FTemperatureKey& Key = Entry.Key;
			const FTemperatureContribution Contribution = Entry.Value;

			const FGameplayTag BoneTag = Key.BodyPart;

			BoneFlowAccum.FindOrAdd(BoneTag) += Contribution.Flow;

			// Center point temperatures around the ambient temperature
			BoneTempAccum.FindOrAdd(BoneTag) += Contribution.Temperature - AmbientTemperature;
		}

		for (const TObjectPtr<UFretteBodyPartData>& Data : BodyPartComponent->BodyPartData)
		{
			if (Data->BodyPartTag.IsValid())
			{
				FGameplayTag BoneTag = Data->BodyPartTag;
				float NetFlow = BoneFlowAccum.FindChecked(BoneTag);
				float NetTemperature = BoneTempAccum.FindChecked(BoneTag) + AmbientTemperature;

				// Truncate the net temp just to be safe
				NetTemperature = FMath::Clamp(NetTemperature, WorldSettings->MinTemperature, WorldSettings->MaxTemperature);

				float CurrentTemp = BodyPartComponent->GetValueFromBodyPart(BoneTag, TemperatureEffectTag);

				if (CurrentTemp < WorldSettings->MinTemperature || CurrentTemp > WorldSettings->MaxTemperature)
				{
					UE_LOG(LogFrette, Log, TEXT("Your temperature system is not doing good in terms of numerical stability no cap!! Truncating temp biatch!!!"));
					continue;
				}

				const float TempDiff = FMath::Abs(NetTemperature - CurrentTemp);

				// When correcting overshooting or returning to ambient temp, big temperature differences can take ages to close up
				// so we will make big differences decrease the ambient flow faster
				const float DynamicAmbient = AmbientFlow * (1.f * (TempDiff / 30.f));

				float EffectiveFlow = 0.0f;
				if (NetFlow > 0 && CurrentTemp > NetTemperature)
				{
					// Flow contributions want to increase temp beyond the target temp so let's go back down with ambient flow
					EffectiveFlow = -DynamicAmbient;
				}
				else if (NetFlow < 0 && CurrentTemp < NetTemperature)
				{
					// Flow contributions want to decrease temp beyond the target temp so let's go back up with ambient flow
					EffectiveFlow = DynamicAmbient;
				}
				else if (NetFlow == 0 && CurrentTemp > NetTemperature)
				{
					// No flow contributions and we need to go down to the target temp
					EffectiveFlow = -DynamicAmbient;
				}
				else if (NetFlow == 0 && CurrentTemp < NetTemperature)
				{
					// No flow contributions and we need to go up to the target temp
					EffectiveFlow = DynamicAmbient;
				}
				else
				{
					// Flow contributions are pushing the temp towards the target so let's use them
					EffectiveFlow = NetFlow;
				}

				// If the difference between the current and target temperature is under DampingThreshold, a damping
				// factor is going to reduce the impact of the temperature delta. This makes the behaviour asymptotic
				// around the target temperature
				const float Damping = FMath::Min(TempDiff / DampingThreshold, 1.0f);
				float DeltaTemp = EffectiveFlow * Damping;

				// Diffusion from neighboring bone tags
				float BoneTagNeighboursFlow = 0;
				for (const auto& Tag : BoneTagNeighbours.FindChecked(BoneTag))
				{
					const float TagTemp = BodyPartComponent->GetValueFromBodyPart(Tag, TemperatureEffectTag);
					BoneTagNeighboursFlow += FMath::Sign(TagTemp - CurrentTemp) * BoneTagNeighboursDiffusionFlow;
				}
				
				DeltaTemp += BoneTagNeighboursFlow;
				DeltaTemp *= TimeBetweenTemperatureChange;

				// TODO: Remove this log spam lol
				UE_LOG(LogFrette, Log, TEXT("Temp integrator: BoneTag[%s] CurrentTemp[%.3f] NetTemp[%.3f] NetFlow[%.3f] AmbientFlow[%.3f] DynamicAmbient[%.3f] EffectiveFlow[%.3f] BoneTagNeighboursFlow[%.3f] DeltaTime[%.3f] Damping[%.3f] DeltaTemp[%.3f]"),
					*BoneTag.ToString(), CurrentTemp, NetTemperature, NetFlow, AmbientFlow, DynamicAmbient, EffectiveFlow, BoneTagNeighboursFlow, TimeBetweenTemperatureChange, Damping, DeltaTemp);

				BodyPartComponent->AddValueFromBodyPartTag(
					BoneTag,
					DeltaTemp,
					TemperatureEffectTag
					);
			}
		}
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureContribution(const FTemperatureContribution Contribution, const FGameplayTag BodyPartTag, FGuid SourceId)
{
	FTemperatureContribution& CurrentValue = BodyPartTemperatureContributions.FindOrAdd(FTemperatureKey(BodyPartTag, SourceId));
	CurrentValue = Contribution;
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