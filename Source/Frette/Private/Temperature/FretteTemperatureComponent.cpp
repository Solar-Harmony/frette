#include "Components/FretteTemperatureComponent.h"

#include "Character/FrettePlayerCharacter.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Frette/Frette.h"
#include "GameFramework/GameStateBase.h"
#include "Weather/FretteWeatherComponent.h"

UFretteTemperatureComponent::UFretteTemperatureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	SetComponentTickInterval(WorldSettings->TimeBeforeTemperatureUpdates);

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

		const float AmbientTemperature = GetSafeAmbientTemperature(GetWorld());

		TMap<FGameplayTag, float> BoneFlowAccum;
		TMap<FGameplayTag, float> BoneTempAccum;

		// We will wait until all calculations are done to update the bone tag temps to avoid
		// synchronization issues and slight oscillations
		// This is the same idea of snapshotting we do in parallelism
		TMap<FGameplayTag, float> BoneDeltaTemp;

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

				const float Temperature = BodyPartComponent->GetValueFromBodyPart(BoneTag, TemperatureEffectTag);
				const float ThermalImpedance = BodyPartComponent->GetValueFromBodyPart(BoneTag, ThermalImpedanceEffectTag);

				if (Temperature < WorldSettings->MinTemperature || Temperature > WorldSettings->MaxTemperature)
				{
					UE_LOG(LogFrette, Log, TEXT("Your temperature system is not doing good in terms of numerical stability no cap!! Truncating temp biatch!!!"));
					continue;
				}

				const float TempDiff = FMath::Abs(NetTemperature - Temperature);

				// When correcting overshooting or returning to ambient temp, big temperature differences can take ages to close up
				// so we will make big differences cause the ambient flow to be faster
				const float DynamicAmbient = AmbientFlow * (1.f + (TempDiff / 30.f));

				// If there are active buffers, we find the max thermal impedance out of them and apply it to DynamicAmbient. Note
				// that this is so slightly overlapping buffers are not a problem. There is no full support for mixing buffers because
				// that kinda doesn't make sense anyway.
				float MaxBufferThermalImpedance = 0;
				for (const auto Imp : BufferThermalImpedances)
					MaxBufferThermalImpedance = FMath::Max(MaxBufferThermalImpedance, Imp.Value);
				const float ConstrainedAmbient = (1.f - MaxBufferThermalImpedance) * DynamicAmbient;

				float EffectiveFlow = 0.0f;
				bool bUsingNetFlow = false;
				if (NetFlow > 0 && Temperature > NetTemperature)
				{
					// Flow contributions want to increase temp beyond the target temp so let's go back down with ambient flow
					EffectiveFlow = -ConstrainedAmbient;
				}
				else if (NetFlow < 0 && Temperature < NetTemperature)
				{
					// Flow contributions want to decrease temp beyond the target temp so let's go back up with ambient flow
					EffectiveFlow = ConstrainedAmbient;
				}
				else if (NetFlow == 0 && Temperature > NetTemperature)
				{
					// No flow contributions and we need to go down to the target temp
					EffectiveFlow = -ConstrainedAmbient;
				}
				else if (NetFlow == 0 && Temperature < NetTemperature)
				{
					// No flow contributions and we need to go up to the target temp
					EffectiveFlow = ConstrainedAmbient;
				}
				else
				{
					// Flow contributions are pushing the temp towards the target so let's use them
					EffectiveFlow = NetFlow;
					bUsingNetFlow = true;
				}

				// If the difference between the current and target temperature is under DampingThreshold, a damping
				// factor is going to reduce the impact of the temperature delta. This makes the behaviour asymptotic
				// around the target temperature
				const float Damping = FMath::Min(TempDiff / DampingThreshold, 1.0f);
				float DeltaTemp = EffectiveFlow * Damping;

				// Diffusion from neighboring bone tags
				float BoneTagNeighboursFlow = 0;
				FGameplayTagContainer* BoneTagNeighbours = BodyPartComponent->BoneTagNeighbours.Find(BoneTag);
				if (BoneTagNeighbours != nullptr)
				{
					for (const auto& Tag : BodyPartComponent->BoneTagNeighbours.FindChecked(BoneTag))
					{
						const float TagTemp = BodyPartComponent->GetValueFromBodyPart(Tag, TemperatureEffectTag);
						const float TagThermalImpedance = BodyPartComponent->GetValueFromBodyPart(Tag, ThermalImpedanceEffectTag);
						// Since a thermal impedance of 1 ruins the system, we will make sure it's capped at 0.95
						BoneTagNeighboursFlow += (1.f - FMath::Lerp(0.f, 0.95f, TagThermalImpedance))
							* (TagTemp - Temperature) * BoneTagNeighboursDiffusionFlow;
					}
				}
				// Huge hack to cut off diffusion if it is conflicting with the flow
				// TODO Fix diffusion that makes the whole thing balance with the other flows and reach false equilibrium points
				// We will probably need a conflict factor and interpolation
				//DeltaTemp += BoneTagNeighboursFlow;

				// Wearing clothes shouldn't affect our capacity to heat up right?
				if (!bUsingNetFlow || NetFlow < 0.f)
					DeltaTemp *= 1.f - FMath::Lerp(0.f, 0.95f, ThermalImpedance);

				DeltaTemp *= WorldSettings->TimeBeforeTemperatureUpdates;

				#if WITH_EDITORONLY_DATA
				if (Cast<AFrettePlayerCharacter>(GetOwner()))
				{
					// Only log spam for the player
					UE_LOG(LogFrette, Log, TEXT("Temp integrator: Tag[%s] Temp[%.3f] NetTemp[%.3f] NetFlow[%.3f] AmbientFlow[%.3f] DynamicAmbientFlow[%.3f] ConstrainedAmbientFlow[%.3f] EffectiveFlow[%.3f] NeighboursFlow[%.3f] DeltaTime[%.3f] Damping[%.3f] DeltaTemp[%.3f]"),
						*(BoneTag.ToString().Replace(TEXT("Frette.BodyPart."), TEXT(""))), Temperature, NetTemperature, NetFlow, AmbientFlow, DynamicAmbient, ConstrainedAmbient, EffectiveFlow, BoneTagNeighboursFlow, WorldSettings->TimeBeforeTemperatureUpdates, Damping, DeltaTemp);
				}
				#endif

				BoneDeltaTemp.Add(BoneTag, DeltaTemp);
			}
		}
		for (const TObjectPtr<UFretteBodyPartData>& Data : BodyPartComponent->BodyPartData)
		{
			if (Data->BodyPartTag.IsValid())
			{
				BodyPartComponent->AddValueFromBodyPartTag(
					Data->BodyPartTag,
					BoneDeltaTemp.FindChecked(Data->BodyPartTag),
					TemperatureEffectTag);
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

void UFretteTemperatureComponent::AddBuffer(UFretteTemperatureBufferComponent* buffer, float ThermalImpedance)
{
	BufferThermalImpedances.Add(buffer, ThermalImpedance);
}

void UFretteTemperatureComponent::ClearBuffer(UFretteTemperatureBufferComponent* buffer)
{
	BufferThermalImpedances.Remove(buffer);
}