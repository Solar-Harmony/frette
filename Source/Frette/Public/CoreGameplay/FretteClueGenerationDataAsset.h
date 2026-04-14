#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FretteClueGenerationDataAsset.generated.h"

UCLASS()
class FRETTE_API UFretteClueGenerationDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// The secondary ratio is the rest
	UPROPERTY(EditAnywhere)
	float PrimaryCluesRatioTarget = 0.3f;
	
	UPROPERTY(EditAnywhere)
	float DudClueRatioTarget = 0.15f;
	
	UPROPERTY(EditAnywhere)
	float MinSecondaryProb = 0.05f;
	
	
	// Greater than 1 will make the ramp concave up more and more
	UPROPERTY(EditAnywhere)
	float RampDegree = 1.2f;
	
	UPROPERTY(EditAnywhere)
	float DesiredGameDuration = 60.f * 20;
	
	
	// No primary clue can ever be given before it has been this amount of time (s) since the last time we got one
	UPROPERTY(EditAnywhere)
	float MinTimeBeforePrimaryClue = 20.f; 
	
	// [0, 1]: the amount of uncertainty when figuring out the next primary clue times with random
	UPROPERTY(EditAnywhere)
	float TimeUncertainty = 0.2f;
};
