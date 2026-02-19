

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "UObject/Interface.h"
#include "InteractibleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractibleInterface : public UInterface
{
	GENERATED_BODY()
};

class FRETTE_API IInteractibleInterface
{
	GENERATED_BODY()

public:
	virtual void BeginHover() = 0;
	virtual void EndHover() = 0;
	virtual void Interact() = 0;
	
	static void SetupOutlineInteractible(UStaticMeshComponent* Mesh);
	static void SetupTextInteractible(UTextRenderComponent* Text);
	static void BeginHoverOutline(UWorld* World, UStaticMeshComponent* Mesh,
		FColor Color, float Thickness = 2.f, float Alpha = 0.7f);
	static void BeginHoverText(UTextRenderComponent* Text);
	static void EndHoverOutline(UStaticMeshComponent* Mesh);
	static void EndHoverText(UTextRenderComponent* Text);
	static void AlignInteractibleText(UWorld* World, UTextRenderComponent* Text);
};
