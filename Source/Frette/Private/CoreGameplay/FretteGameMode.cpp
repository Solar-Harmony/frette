#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "CoreGameplay/FretteMainObjective.h"
#include "CoreGameplay/FretteLandmark.h"
#include "Frette/Frette.h"

void AFretteGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AFretteMainObjective> It(GetWorld()); It; ++It)
	{
		require(MainObjective == nullptr, "Multiple main objectives found in the level! There should be only one.");
		MainObjective = *It;
	}
	
	const FVector2D ObjectiveLocation2D(MainObjective->GetActorLocation());
	const float NearObjectiveRadiusSq = FMath::Square(MainObjective->NearObjectiveRadiusCm);
	
	for (TActorIterator<AFretteLandmark> It(GetWorld()); It; ++It)
	{
		AFretteLandmark* Landmark = *It;
		
		const FVector2D LandmarkLocation2D(Landmark->GetActorLocation());
		const float DistanceToObjectiveSq = FVector2D::DistSquared(LandmarkLocation2D, ObjectiveLocation2D);
		
		if (DistanceToObjectiveSq <= NearObjectiveRadiusSq)
		{
			Landmark->bIsNearMainObjective = true;
			NearLandmarks.Add(Landmark);
		}
		else
		{
			FarLandmarks.Add(Landmark);
		}
	}
}

AFretteLandmark* AFretteGameMode::GetRandomNearLandmark() const
{
	const int32 Idx = FMath::RandRange(0, NearLandmarks.Num() - 1);
	return NearLandmarks[Idx];
}

AFretteLandmark* AFretteGameMode::GetRandomFarLandmark() const
{
	const int32 Idx = FMath::RandRange(0, FarLandmarks.Num() - 1);
	return FarLandmarks[Idx];
}