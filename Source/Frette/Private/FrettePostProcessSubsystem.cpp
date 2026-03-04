#include "FrettePostProcessSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Misc/MapErrors.h"

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
	UWorld* World = GetWorld();

	APostProcessVolume* Volume = Cast<APostProcessVolume>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass())
	);
	if (!IsValid(Volume))
	{
		FMessageLog("MapCheck").Error()
			->AddToken(FTextToken::Create(
				FText::FromString("Missing required UNBOUND PostProcessVolume for UFrettePostProcessSubsystem"
					  "with the interactable material")
			));
		return;
	}

	FString MPCPath = TEXT("/Game/Materials/FretteInteractableMaterialParams.FretteInteractableMaterialParams");
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

void UFrettePostProcessSubsystem::SetOutline(FLinearColor Color, float Thickness, float Alpha)
{
	if (!IsValid(OutlineMPCI))
		return;

	OutlineMPCI->SetVectorParameterValue(TEXT("Color"), Color);
	OutlineMPCI->SetScalarParameterValue(TEXT("Thickness"), Thickness);
	OutlineMPCI->SetScalarParameterValue(TEXT("Alpha"), Alpha);
}