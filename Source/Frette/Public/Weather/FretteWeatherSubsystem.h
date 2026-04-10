#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FretteWeatherSubsystem.generated.h"

UCLASS()
class FRETTE_API UFretteWeatherSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
}