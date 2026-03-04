#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "CoreGameplay/FretteClue.h"
#include "CoreGameplay/FretteLandmark.h"
#include "CoreGameplay/FretteMainObjective.h"
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
	
	for (TActorIterator<AFretteClue> It(GetWorld()); It; ++It)
	{
		++NumInitialClues;
	}
} 

AFretteLandmark* AFretteGameMode::GetRandomLandmark(bool bNearObjective)
{
	auto& Array = bNearObjective ? NearLandmarks : FarLandmarks;
	const int32 Idx = FMath::RandRange(0, Array.Num() - 1);
	AFretteLandmark* Item = Array[Idx];
	Array.RemoveAtSwap(Idx);
	return Item; 		
}