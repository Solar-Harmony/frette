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

	for (const TObjectPtr<UFretteBodyPartData>& Data : BodyPartComponent->BodyPartData)
	{
		if (Data->BodyPartTag.IsValid())
		{
			BodyPartTemperatureTargets.Add(Data->BodyPartTag, 0);
		}
	}
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

		for (const auto BodyPartTemperatureTarget : BodyPartTemperatureTargets)
		{
			const int CurrentBodyPartTemperature = BodyPartComponent->GetValueFromBodyPart(BodyPartTemperatureTarget.Key, TemperatureEffectTag);
			const int TargetTemperature = BodyPartTemperatureTarget.Value + AmbientTemperature;
			if (CurrentBodyPartTemperature == TargetTemperature)
				continue;

			//Pourrais ajouter un modifier pour rendre le changement soit plus rapide ou plus lent sans avoir a changer la tick interval
			const int Delta = TargetTemperature > CurrentBodyPartTemperature ? 1 : -1;

			BodyPartComponent->AddValueFromBodyPartTag(BodyPartTemperatureTarget.Key, Delta, TemperatureEffectTag);
		}
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureModifier(const int NewTargetTemperature, const FGameplayTag BodyPartTag)
{
	if (int* CurrentValue = BodyPartTemperatureTargets.Find(BodyPartTag))
	{
		*CurrentValue += NewTargetTemperature;
	}
}

void UFretteTemperatureComponent::AddBodyPartTemperatureModifier(const int NewTargetTemperature, const FName BoneName)
{
	FGameplayTag BodyPartTag = BodyPartComponent->GetBodyPartFromBoneName(BoneName);

	if (int* CurrentValue = BodyPartTemperatureTargets.Find(BodyPartTag))
	{
		*CurrentValue += NewTargetTemperature;
	}
}

void UFretteTemperatureComponent::AddToAmbientTemperature(const int NewAmbientTemperature)
{
	AmbientTemperature += NewAmbientTemperature;
}