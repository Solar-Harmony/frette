#include "Core/FrettePostProcessSubsystem.h"

#include "Frette.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

void UFrettePostProcessSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UWorld* World = GetWorld())
	{
		World->OnWorldBeginPlay.AddUObject(this, &UFrettePostProcessSubsystem::OnWorldInitialized);
	}
}

void UFrettePostProcessSubsystem::OnWorldInitialized()
{
	const UWorld* World = GetWorld();
	const APostProcessVolume* Volume = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	
	if (!IsValid(Volume) || !Volume->bUnbound)
	{
		FMessageLog Log("Frette");
		Log.Error()->AddText(INVTEXT("A PostProcessVolume with Unbound Extents is required for interactible outlines to work."));
		Log.Notify();
		return;
	}

	// TODO: Don't use hardcoded path
	const FString MPCPath = TEXT("/Game/Materials/FretteInteractableMaterialParams.FretteInteractableMaterialParams");
	const UMaterialParameterCollection* MPC = Cast<UMaterialParameterCollection>(StaticLoadObject(UMaterialParameterCollection::StaticClass(), nullptr, *MPCPath));
	unless(IsValid(MPC), "Failed to load Material Parameter Collection for interactible outlines! Make sure the asset exists at the specified path.")
		return;

	OutlineMPCI = World->GetParameterCollectionInstance(MPC);
}

void UFrettePostProcessSubsystem::SetOutline(FLinearColor Color, float Thickness, float Alpha) const
{
	if (!IsValid(OutlineMPCI))
		return;

	OutlineMPCI->SetVectorParameterValue(TEXT("Color"), Color);
	OutlineMPCI->SetScalarParameterValue(TEXT("Thickness"), Thickness);
	OutlineMPCI->SetScalarParameterValue(TEXT("Alpha"), Alpha);
}