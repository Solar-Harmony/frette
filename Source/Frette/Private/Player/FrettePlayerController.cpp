#include "Player/FrettePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Input/FretteInputComponent.h"

void AFrettePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateInteractableTarget();
}

void AFrettePlayerController::UpdateInteractableTarget()
{
	FHitResult Hit;
	FVector WorldLocation;
	FRotator ViewRotation;
	GetPlayerViewPoint(WorldLocation, ViewRotation);
	const FVector WorldDirection = ViewRotation.Vector();

	const FVector TraceEnd = WorldLocation + WorldDirection * InteractTraceRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractTraceRadius);
	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, WorldLocation, TraceEnd, FQuat::Identity, ECC_Visibility, Sphere, Params);

	TScriptInterface<IInteractibleInterface> NewHovered = nullptr;
	if (bHit && Hit.GetActor())
	{
		if (Hit.GetActor()->Implements<UInteractibleInterface>())
		{
			NewHovered.SetObject(Hit.GetActor());
			NewHovered.SetInterface(Cast<IInteractibleInterface>(Hit.GetActor()));
		}
	}

	if (NewHovered.GetObject() == CurrentHoveredActor.GetObject())
		return;

	if (CurrentHoveredActor.GetInterface())
		CurrentHoveredActor->EndHover();

	CurrentHoveredActor = NewHovered;

	if (CurrentHoveredActor.GetInterface())
		CurrentHoveredActor->BeginHover();
}

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

UFretteAbilitySystemComponent* AFrettePlayerController::GetASC()
{
	if (FretteAbilitySystemComponent == nullptr)
	{
		FretteAbilitySystemComponent = Cast<UFretteAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return FretteAbilitySystemComponent;
}