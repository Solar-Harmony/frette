#include "Components/FretteTemperatureComponent.h"

#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Frette/Frette.h"

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

				// Our sources always either inject or remove heat, so if we are hotter or colder respectively than the target temp, we need to kill the flow
				if ((NetFlow > 0 && CurrentTemp >= NetTemperature)
					|| (NetFlow < 0 && CurrentTemp <= NetTemperature))
					NetFlow = 0;

				const float AmbientFlow = DiffusionSpeed * (AmbientTemperature - CurrentTemp);
				NetFlow += AmbientFlow;

				// Make things more physically accurate??
				const float DeltaTime = GetWorld()->GetDeltaSeconds();
				NetFlow *= DeltaTime;

				// DAMPING: We will make the behavior asymptotic near the target temp to make the system equilibrate
				const float Influence = (NetTemperature - CurrentTemp) / (WorldSettings->MaxTemperature - WorldSettings->MinTemperature);
				const float AsymptoticFlow = FMath::Lerp(1.0f - Damping, 1.0f, FMath::Abs(Influence)) * NetFlow;

				// TODO: Remove this log spam lol
				UE_LOG(LogFrette, Log, TEXT("Temp integrator: BoneTag[%s] CurrentTemp[%.4f] NetTemp[%.4f] NetFlow[%.4f] DeltaTime[%.4f] Influence[%.4f] AsymptoticFlow[%.4f]"),
					*BoneTag.ToString(), CurrentTemp, NetTemperature, NetFlow, DeltaTime, Influence, AsymptoticFlow);

				// Add the computed value to the bone component
				BodyPartComponent->AddValueFromBodyPartTag(
					BoneTag,
					AsymptoticFlow,
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