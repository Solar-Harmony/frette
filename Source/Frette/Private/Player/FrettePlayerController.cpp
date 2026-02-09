#include "Player/FrettePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Input/FretteInputComponent.h"

void AFrettePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
		return;

	check(DefaultInputContext);

	// FIXME: already added in BP
	if (auto* Input = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Input->AddMappingContext(DefaultInputContext, 0);
	}
}

void AFrettePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//Le input component est setter a UFretteInputComponent dans les projects settings donc il peut être cast ici
	UFretteInputComponent* FretteInputComponent = CastChecked<UFretteInputComponent>(InputComponent);
	FretteInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased);
}

void AFrettePlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC())
		GetASC()->AbilityInputPressed(InputTag);
}

void AFrettePlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC())
		GetASC()->AbilityInputReleased(InputTag);
}

void AFrettePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	
}

UFretteAbilitySystemComponent* AFrettePlayerController::GetASC()
{
	if (FretteAbilitySystemComponent == nullptr)
	{
		FretteAbilitySystemComponent = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return FretteAbilitySystemComponent;
}