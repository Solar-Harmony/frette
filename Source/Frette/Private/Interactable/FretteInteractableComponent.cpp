#include "Interactable/FretteInteractableComponent.h"

void UFretteInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Mesh))
	{
		Mesh->SetRenderCustomDepth(false);
	}
}