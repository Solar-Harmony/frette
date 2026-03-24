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

//Le target temperature devras être par partie du corps selon les vetement équippé
//TODO: Il faudrais peut-être un tempereature target par bodypart vu que des éffets pourrais affecter une bodypart spécifique
//Marcher dans l'eau/neige qui refroidis les parties qui touche ceux-ci par exemple
void UFretteTemperatureComponent::OnTemperatureTick()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (!BodyPartComponent || CurrentTemperature == TargetTemperature)
			return;

		const int StackDelta = TargetTemperature > CurrentTemperature ? 1 : -1;

		CurrentTemperature += StackDelta;

		BodyPartComponent->AddValueToAllParts(StackDelta, TemperatureEffectTag);
	}
}