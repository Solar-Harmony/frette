#include "Interactable/FretteInteractorComponent.h"

#include "FrettePostProcessSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Interactable/FretteInteractableComponent.h"
#include "Components/TextBlock.h"
#include "Player/FrettePlayerController.h"

UFretteInteractorComponent::UFretteInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
			
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);

			InteractWidgetInstance->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
			InteractWidgetInstance->SetPositionInViewport(ViewportSize
				* FVector2D(0.5f, 0.35f));
			
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
	if (CurrentHoveredActor.GetInterface())
	{
		UFretteInteractableComponent* Interactable = CurrentHoveredActor->GetInteractableComponent();
		Interactable->OnInteract.Broadcast();
	}
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

	if (NewHovered.GetObject() == CurrentHoveredActor.GetObject())
		return;

	if (CurrentHoveredActor.GetInterface())
	{
		UFretteInteractableComponent* Interactable = CurrentHoveredActor->GetInteractableComponent();
		
		if (IsValid(InteractWidgetInstance) && Interactable->bShowMessage)
		{
			InteractWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
		
		if (Interactable->bShowOutline && IsValid(Interactable->Mesh))
		{
			Interactable->Mesh->SetRenderCustomDepth(false);
		}
		
		Interactable->OnEndHover.Broadcast();
	}

	CurrentHoveredActor = NewHovered;

	if (CurrentHoveredActor.GetInterface())
	{
		UFretteInteractableComponent* Interactable = CurrentHoveredActor->GetInteractableComponent();
		
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

