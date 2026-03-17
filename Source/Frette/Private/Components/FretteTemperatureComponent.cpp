#include "Components/FretteTemperatureComponent.h"

#include "Components/BodyPart/FretteBodyPartComponent.h"

void UFretteTemperatureComponent::BeginPlay()
{
	Super::BeginPlay();

	BodyPartComponent = GetOwner()->GetComponentByClass<UFretteBodyPartComponent>();

	// Only drive stacks from the server
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TemperatureTickHandle,
			this,
			&UFretteTemperatureComponent::OnTemperatureTick,
			TimeBetweenTemperatureChange,
			true // looping
			);
	}
}

//Le target temperature devras être par partie du corps selon les vetement équippé
//TODO: Il faudrais peut-être un tempereature target par bodypart vu que des éffets pourrais affecter une bodypart spécifique
//Marcher dans l'eau/neige qui refroidis les parties qui touche ceux-ci par exemple
void UFretteTemperatureComponent::OnTemperatureTick()
{
	if (!BodyPartComponent || CurrentTemperatureStacks == TargetTemperature)
		return;

	// Move one step toward TargetTemperature
	const int StackDelta = TargetTemperature > CurrentTemperatureStacks ? 1 : -1;

	CurrentTemperatureStacks += StackDelta;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current body temperature: %d"), CurrentTemperatureStacks));

	BodyPartComponent->AddValueToAllParts(StackDelta, TemperatureEffectTag);
}