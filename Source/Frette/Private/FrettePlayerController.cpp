#include "Frette/Public/FrettePlayerController.h"

#include "EnhancedInputSubsystems.h"

void AFrettePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
		return;
	
	check(DefaultInputContext);
	
	if (auto* Input = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Input->AddMappingContext(DefaultInputContext, 0);
	}
	// create widgets
	// create VMs for the widgets
	// initialize the VMs with some data
	// set the VMs on the widgets
	// add the widgets to the viewport
	
	// problems:
	// we need to pass the widget class through UPROPERTY, how to match it with the VM class?
	// and how to provide the data to the VM?
}
