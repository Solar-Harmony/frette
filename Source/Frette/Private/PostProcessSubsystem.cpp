#include "PostProcessSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

void UPostProcessSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UWorld* World = GetWorld())
	{
		World->OnWorldBeginPlay.AddUObject(this, &UPostProcessSubsystem::OnWorldInitialized);
	}
}

void UPostProcessSubsystem::OnWorldInitialized()
{
	UWorld* World = GetWorld();
	
	FString MPCPath = TEXT("/Game/Materials/InteractibleParams.InteractibleParams");
	UMaterialParameterCollection* MPC = Cast<UMaterialParameterCollection>(
		StaticLoadObject(UMaterialParameterCollection::StaticClass(), nullptr, *MPCPath)
	);
	if (!IsValid(MPC))
	{
		UE_LOG(LogTemp, Warning, TEXT("PostProcessSubsystem: Failed to load MPC at path: %s"), *MPCPath);
	}
	else
	{
		OutlineMPCI = World->GetParameterCollectionInstance(MPC);
	}
}

void UPostProcessSubsystem::SetOutline(FLinearColor Color, float Thickness, float Alpha)
{
	if (!IsValid(OutlineMPCI))
		return;

	OutlineMPCI->SetVectorParameterValue(TEXT("Color"), Color);
	OutlineMPCI->SetScalarParameterValue(TEXT("Thickness"), Thickness);
	OutlineMPCI->SetScalarParameterValue(TEXT("Alpha"), Alpha);
}
