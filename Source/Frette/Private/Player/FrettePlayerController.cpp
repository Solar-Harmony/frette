#include "Player/FrettePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Input/FretteInputComponent.h"
#include "Player/FrettePlayerState.h"

class AFrettePlayerState;

AFrettePlayerController::AFrettePlayerController()
{
	Interactor = CreateDefaultSubobject<UFretteInteractorComponent>("Interactor");
}

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

void AFrettePlayerController::Client_OnClueGenerated_Implementation(const FText& ClueText)
{
	OnClientReceiveNewClue.Broadcast(ClueText);
}

void AFrettePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//Le input component est setter a UFretteInputComponent dans les projects settings donc il peut être cast ici
	UFretteInputComponent* FretteInputComponent = CastChecked<UFretteInputComponent>(InputComponent);
	FretteInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased);
	FretteInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::OnInteractPressed);
}

// this is needed by the listen server, because since it is both client and server
// it will not replicate to itself and thus won't trigger OnRep callbacks
void AFrettePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (IsLocalController())
	{
		SetupWidgetsAndViewModels();
	}
}

void AFrettePlayerController::OnInteractPressed()
{
	Interactor->Interact();
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

	if (IsLocalController())
	{
		SetupWidgetsAndViewModels();
	}
}

UFretteAbilitySystemComponent* AFrettePlayerController::GetASC()
{
	if (FretteAbilitySystemComponent == nullptr)
	{
		FretteAbilitySystemComponent = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return FretteAbilitySystemComponent;
}