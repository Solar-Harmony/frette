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
}
