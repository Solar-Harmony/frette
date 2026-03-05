#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "CoreGameplay/FretteClue.h"
#include "CoreGameplay/FretteLandmark.h"
#include "CoreGameplay/FretteMainObjective.h"
#include "Frette/Frette.h"

FText AFretteGameMode::GenerateClue(AFrettePlayerCharacter* Interactor, float DudClueChance, float Steepness, float Midpoint)
{
	FText ClueText;
	
	if (FMath::FRand() < DudClueChance)
	{
		ClueText = INVTEXT("This is some fascinating lore.");
	}
	else
	{
		const bool bIsPrimaryClue = ShouldPickPrimaryClue(Steepness, Midpoint);
		const AFretteLandmark* POI = GetRandomLandmark(bIsPrimaryClue);
		
		if (bIsPrimaryClue)
		{
			ClueText = FText::Format(INVTEXT("The main objective is near a {0}."), POI->DisplayName);
		}
		else
		{
			ClueText = FText::Format(INVTEXT("An interesting point of interest with loot is near a {0}."), POI->DisplayName);
		}
	}
	
	return ClueText;
}

void AFretteGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AFretteMainObjective> It(GetWorld()); It; ++It)
	{
		require(MainObjective == nullptr, "Multiple main objectives found in the level! There should be only one.");
		MainObjective = *It;
	}
	
	require(IsValid(MainObjective), "No main objective found in the level! Make sure to place one in the level.");
	
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

bool AFretteGameMode::ShouldPickPrimaryClue(float Steepness, float Midpoint) const
{
	const float NormalizedClueCount = static_cast<float>(NumCluesFound) / NumInitialClues;
	
	auto Sigmoid = [Steepness, Midpoint](float N) 
	{
		return 1.0f / (1.0f + FMath::Exp(-Steepness * (N - Midpoint)));
	};
	
	// sigmoid gives an asymptotic S-curve but we want exact an exact [0,1] domain so we normalize
	static const float MinCurveVal = Sigmoid(0.0f);
	static const float MaxCurveVal = Sigmoid(1.0f);
	const float CurveVal = Sigmoid(NormalizedClueCount);
	const float Probability = (CurveVal - MinCurveVal) / (MaxCurveVal - MinCurveVal);
	
	return FMath::FRand() < Probability;
}

AFretteLandmark* AFretteGameMode::GetRandomLandmark(bool bNearObjective)
{
	// TODO: Need to handle when no more clues!
	auto& Array = bNearObjective ? NearLandmarks : FarLandmarks;
	const int32 Idx = FMath::RandRange(0, Array.Num() - 1);
	AFretteLandmark* Item = Array[Idx];
	Array.RemoveAtSwap(Idx);
	return Item; 		
}