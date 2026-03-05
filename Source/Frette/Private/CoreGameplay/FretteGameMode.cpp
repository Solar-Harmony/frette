#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "CoreGameplay/FretteClue.h"
#include "CoreGameplay/FretteLandmark.h"
#include "CoreGameplay/FretteMainObjective.h"
#include "Frette/Frette.h"

FText AFretteGameMode::GenerateClue(const AFrettePlayerCharacter* Interactor, float DudClueChance)
{
	FText ClueText;
	
	if (FMath::FRand() < DudClueChance)
	{
		ClueText = INVTEXT("This is some fascinating lore.");
	}
	else
	{
		const bool bIsPrimaryClue = ShouldPickPrimaryClue();
		const AFretteLandmark* POI = GetRandomLandmark(bIsPrimaryClue);
		if (POI == nullptr)
		{
			ClueText = INVTEXT("This is some fascinating lore.");
			UE_LOG(LogFrette, Warning, TEXT("Failed to generate clue: no more landmarks available!"));
			return ClueText;
		}
		
		if (bIsPrimaryClue)
		{
			ClueText = FText::Format(INVTEXT("The main objective is near a {0}."), POI->DisplayName);
		}
		else
		{
			const FVector2D Direction((POI->GetActorLocation() - Interactor->GetActorLocation()).GetSafeNormal());
			const FText Cardinal = FText::FromString(DirVectorToCardinal(Direction));
			ClueText = FText::Format(INVTEXT("An interesting point of interest with loot {0} of a {1}."), Cardinal, POI->DisplayName);
		}
		
		UE_LOG(LogFrette, Log, TEXT("Clue generated. Leads to landmark: %s, is near objective: %d"), *POI->GetName(), bIsPrimaryClue);
	}
	
	NumCluesDiscovered++;
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
		++NumCluesGenerated;
	}
	
	NumNearCluesGenerated = NearLandmarks.Num();
}

bool AFretteGameMode::ShouldPickPrimaryClue() const
{
	check(NumCluesDiscovered <= NumCluesGenerated);
	
	// it must be impossible to find all clues and not get all the primary clues.
	const float ClueRatio = static_cast<float>(NumCluesDiscovered) / (NumCluesGenerated - NumNearCluesGenerated);
	if (ClueRatio >= 1.0f)
		return true;
	
	// simple linear function with min clamped so first clues can never be good
	// this should be good enough, i think lol
	const float Probability = FMath::Clamp(ClueRatio, 0.1f, 1.0f);
	UE_LOG(LogFrette, Log, TEXT("Found clue %d of %d. Probability to pick primary clue: %f."), NumCluesDiscovered, NumCluesGenerated, Probability);
	
	return FMath::FRand() < Probability;
}

AFretteLandmark* AFretteGameMode::GetRandomLandmark(bool bNearObjective)
{
	auto& Array = bNearObjective ? NearLandmarks : FarLandmarks;
	if (Array.IsEmpty())
		return nullptr;
	
	const int32 Idx = FMath::RandRange(0, Array.Num() - 1);
	AFretteLandmark* Item = Array[Idx];
	Array.RemoveAtSwap(Idx);
	return Item; 		
}

FString AFretteGameMode::DirVectorToCardinal(const FVector2D& Dir)
{
	check(!Dir.IsNearlyZero());
	
	const float Angle = FMath::Atan2(Dir.Y, Dir.X); // get vector angle between -180 and 180 deg
	constexpr float SectorSize = PI / 4; // divide circle in 8 sectors, 45 deg each
	const int Sector = FMath::RoundToInt(Angle / SectorSize); // round to nearest sector -> [-4, 4]
	const int SectorIdx = (Sector % 8 + 8) % 8; // remap to [0, 7]
	
	static const char* Names[8] =
	{
		"east",
		"north-east",
		"north",
		"north-west",
		"west",
		"south-west",
		"south",
		"south-east"
	};

	return Names[SectorIdx];
}