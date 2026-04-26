#include "Interactable/FretteInteractorComponent.h"

#include "FrettePostProcessSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Character/FrettePlayerCharacter.h"
#include "Components/TextBlock.h"
#include "Frette/Frette.h"
#include "Interactable/FretteInteractableComponent.h"
#include "Player/FrettePlayerController.h"

UFretteInteractorComponent::UFretteInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UFretteInteractableComponent* UFretteInteractorComponent::GetInteractableComponentFromHover(TScriptInterface<IFretteInteractableInterface> hover) const
{
	UFretteInteractableComponent* Interactable = nullptr;

	// GetInterface() is only non-null for C++ implementors; Blueprint implementors return null here.
	// So we skip the virtual call and go straight to FindComponentByClass for both paths.
	if (hover.GetInterface())
	{
		Interactable = hover->GetInteractableComponent();
	}

	if (!IsValid(Interactable))
	{
		AActor* Actor = Cast<AActor>(hover.GetObject());
		if (!IsValid(Actor))
			return nullptr;

		Interactable = Actor->FindComponentByClass<UFretteInteractableComponent>();
		if (!IsValid(Interactable))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hovered actor %s implements IFretteInteractableInterface but has no valid InteractableComponent"), *Actor->GetName());
			return nullptr;
		}
	}
	return Interactable;
}

void UFretteInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AFrettePlayerController>(GetOwner());
	if (!PlayerController->IsLocalPlayerController())
		return;

	if (InteractWidgetClass)
	{
		InteractWidgetInstance = CreateWidget<UUserWidget>(PlayerController, InteractWidgetClass);
		if (IsValid(InteractWidgetInstance))
		{
			InteractWidgetInstance->AddToViewport();
			InteractWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UFretteInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateInteractableTarget();
}

void UFretteInteractorComponent::Interact()
{
	if (CurrentHoveredActor.GetObject())
	{
		UFretteInteractableComponent* Interactable = GetInteractableComponentFromHover(CurrentHoveredActor);
		if (IsValid(Interactable))
		{
			CurrentInteractable = Interactable;
			Server_Interact(Interactable->GetOwner()); // TODO: Suboptimal
		}
	}
}

void UFretteInteractorComponent::Server_Interact_Implementation(AActor* Interactable)
{
	// TODO: the server should validate by raycasting again, to prevent cheating
	unless(IsValid(Interactable)) return;

	const UFretteInteractableComponent* InteractableComponent = Interactable->GetComponentByClass<UFretteInteractableComponent>();
	unless(IsValid(InteractableComponent)) return;

	AFrettePlayerCharacter* Interactor = PlayerController->GetPawn<AFrettePlayerCharacter>();
	unless(IsValid(Interactor)) return;

	InteractableComponent->OnInteract.Broadcast(Interactor);
}

void UFretteInteractorComponent::EndInteract()
{
	if (!CurrentInteractable)
		return;

	const UFretteInteractableComponent* Interactable = GetInteractableComponentFromHover(CurrentHoveredActor);
	if (IsValid(Interactable))
	{
		CurrentInteractable = nullptr;
		Server_EndInteract(Interactable->GetOwner());
	}
}

void UFretteInteractorComponent::Server_EndInteract_Implementation(AActor* Interactable)
{
	unless(IsValid(Interactable)) return;

	const UFretteInteractableComponent* InteractableComponent = Interactable->GetComponentByClass<UFretteInteractableComponent>();
	unless(IsValid(InteractableComponent)) return;

	AFrettePlayerCharacter* Interactor = PlayerController->GetPawn<AFrettePlayerCharacter>();
	unless(IsValid(Interactor)) return;

	InteractableComponent->OnInteractEnd.Broadcast(Interactor);
}

void UFretteInteractorComponent::UpdateInteractableTarget()
{
	if (!IsValid(PlayerController))
		return;

	FHitResult Hit;
	FVector WorldLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(WorldLocation, ViewRotation);
	const FVector WorldDirection = ViewRotation.Vector();

	const FVector TraceEnd = WorldLocation + WorldDirection * InteractTraceRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerController->GetPawn());

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractTraceRadius);
	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, WorldLocation, TraceEnd, FQuat::Identity, ECC_Visibility, Sphere, Params);

	TScriptInterface<IFretteInteractableInterface> NewHovered = nullptr;
	if (bHit && Hit.GetActor())
	{
		if (Hit.GetActor()->Implements<UFretteInteractableInterface>())
		{
			NewHovered.SetObject(Hit.GetActor());
			NewHovered.SetInterface(Cast<IFretteInteractableInterface>(Hit.GetActor()));
		}
	}

	const bool bCurrentValid = IsValid(CurrentHoveredActor.GetObject());

	if (bCurrentValid && NewHovered.GetObject() == CurrentHoveredActor.GetObject())
		return;

	if (CurrentHoveredActor.GetObject())
	{
		if (IsValid(InteractWidgetInstance))
		{
			InteractWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}

		if (bCurrentValid)
		{
			UFretteInteractableComponent* Interactable = GetInteractableComponentFromHover(CurrentHoveredActor);
			if (IsValid(Interactable))
			{
				if (Interactable->bShowOutline && IsValid(Interactable->Mesh))
				{
					Interactable->Mesh->SetRenderCustomDepth(false);
				}
				Interactable->OnEndHover.Broadcast();
			}
		}
	}

	CurrentHoveredActor = NewHovered;

	if (CurrentHoveredActor.GetObject())
	{
		UFretteInteractableComponent* Interactable = GetInteractableComponentFromHover(CurrentHoveredActor);

		if (IsValid(Interactable))
		{
			if (IsValid(InteractWidgetInstance) && Interactable->bShowMessage)
			{
				InteractWidgetInstance->SetVisibility(ESlateVisibility::Visible);
				UTextBlock* TextLabel = Cast<UTextBlock>(InteractWidgetInstance->GetWidgetFromName(TEXT("TextBlock_Message")));
				if (IsValid(TextLabel))
				{
					TextLabel->SetText(Interactable->Message);
				}
			}

			if (Interactable->bShowOutline && IsValid(Interactable->Mesh))
			{
				GetWorld()->GetGameInstance()
				          ->GetSubsystem<UFrettePostProcessSubsystem>()
				          ->SetOutline(Interactable->OutlineColor, Interactable->OutlineThickness, Interactable->OutlineAlpha);
				Interactable->Mesh->SetCustomDepthStencilValue(2);
				Interactable->Mesh->SetRenderCustomDepth(true);
			}

			Interactable->OnBeginHover.Broadcast();
		}
	}
}