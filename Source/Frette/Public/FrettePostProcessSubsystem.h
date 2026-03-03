#pragma once

#include "FrettePostProcessSubsystem.generated.h"

UCLASS()
class FRETTE_API UFrettePostProcessSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UMaterialInstanceDynamic* InteractibleMID;

	UPROPERTY()
	UMaterialParameterCollectionInstance* OutlineMPCI;

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnWorldInitialized();

	void SetOutline(FLinearColor Color, float Thickness, float Alpha);
};