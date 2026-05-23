#pragma once

#include "FrettePostProcessSubsystem.generated.h"

UCLASS()
class FRETTE_API UFrettePostProcessSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> InteractibleMID;

	UPROPERTY()
	TObjectPtr<UMaterialParameterCollectionInstance> OutlineMPCI;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnWorldInitialized();

	void SetOutline(FLinearColor Color, float Thickness, float Alpha) const;
};