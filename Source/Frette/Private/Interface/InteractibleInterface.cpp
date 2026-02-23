#include "Interface/InteractibleInterface.h"

#include "PostProcessSubsystem.h"
#include "Kismet/GameplayStatics.h"

void IInteractibleInterface::SetupTextInteractible(UTextRenderComponent* Text)
{
	Text->SetVisibility(false);
	Text->SetHorizontalAlignment(EHTA_Center);
	Text->SetVerticalAlignment(EVRTA_TextCenter);
	Text->SetWorldSize(25.f);
	Text->SetTextRenderColor(FColor::White);
}

void IInteractibleInterface::SetupOutlineInteractible(UStaticMeshComponent* Mesh)
{
	Mesh->SetRenderCustomDepth(false);
}

void IInteractibleInterface::BeginHoverOutline(UWorld* World, UStaticMeshComponent* Mesh,
	FColor Color, float Thickness, float Alpha)
{
	World->GetGameInstance()
	     ->GetSubsystem<UPostProcessSubsystem>()
	     ->SetOutline(FLinearColor(1.f, 0.f, 0.f), Thickness, Alpha);

	Mesh->SetCustomDepthStencilValue(2);
	Mesh->SetRenderCustomDepth(true);
}

void IInteractibleInterface::BeginHoverText(UTextRenderComponent* Text)
{
	Text->SetVisibility(true);
}

void IInteractibleInterface::EndHoverText(UTextRenderComponent* Text)
{
	Text->SetVisibility(false);
}

void IInteractibleInterface::EndHoverOutline(UStaticMeshComponent* Mesh)
{
	Mesh->SetRenderCustomDepth(false);
}

void IInteractibleInterface::AlignInteractibleText(UWorld* World, UTextRenderComponent* Text)
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
	if (IsValid(CameraManager))
	{
		FVector CameraLocation = CameraManager->GetCameraLocation();
		FVector DirectionToCamera = CameraLocation - Text->GetComponentLocation();
		DirectionToCamera.Z = 0.f;

		FRotator LookAtRot = DirectionToCamera.Rotation();
		Text->SetRelativeRotation(FRotator(0.f, LookAtRot.Yaw, 0.f)); 
	}
}