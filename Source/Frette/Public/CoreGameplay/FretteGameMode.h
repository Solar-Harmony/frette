#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FretteGameMode.generated.h"

class AFretteMainObjective;
class AFretteLandmark;

/**
 * The main game loop logic for Frette. Caches the actors used for the clue system.
 */
UCLASS()
class AFretteGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure)
	AFretteMainObjective* GetMainObjective() const { return MainObjective; }
	
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Random Landmark Near Main Objective"))
	AFretteLandmark* GetRandomNearLandmark() const;
	
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Random Landmark Far From Main Objective"))
	AFretteLandmark* GetRandomFarLandmark() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<AFretteMainObjective> MainObjective;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> NearLandmarks;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AFretteLandmark>> FarLandmarks;
};