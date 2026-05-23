#include "GPI/Interactable/FretteInteractableComponent.h"

#include "Net/UnrealNetwork.h"

UFretteInteractableComponent::UFretteInteractableComponent()
{
	SetIsReplicatedByDefault(true);
}

void UFretteInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Mesh))
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

void UFretteInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFretteInteractableComponent, bShowOutline);
	DOREPLIFETIME(UFretteInteractableComponent, bShowMessage);
}