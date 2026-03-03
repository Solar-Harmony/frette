#include "Interactable/FretteInteractableComponent.h"

UFretteInteractableComponent::UFretteInteractableComponent() {}

void UFretteInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Mesh))
	{
		Mesh->SetRenderCustomDepth(false);
	}
}