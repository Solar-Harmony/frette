#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FretteGameMode.generated.h"

class AFretteMainObjective;
class AFrettePOI;

/**
 * Manager for caching the POIs in the level.
 */
UCLASS()
class AFretteGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFrettePOI>> LocationsNearObjective;
};