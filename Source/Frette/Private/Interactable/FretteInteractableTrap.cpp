#include "Interactable/FretteInteractableTrap.h"

#include "Interactable/FretteInteractableComponent.h"

// Sets default values
AFretteInteractableTrap::AFretteInteractableTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	Interactable = CreateDefaultSubobject<UFretteInteractableComponent>(TEXT("Interactable Component"));
	Interactable->bShowMessage = true;
	Interactable->bShowOutline = true;
	Interactable->OutlineColor = FColor::Blue;

}

void AFretteInteractableTrap::BeginPlay()
{
	Super::BeginPlay();

}

void AFretteInteractableTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}