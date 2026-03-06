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
			NumPrimaryCluesFound++;
		}
		else
		{
			const FVector2D Direction((POI->GetActorLocation() - Interactor->GetActorLocation()).GetSafeNormal());
			const FText Cardinal = FText::FromString(DirVectorToCardinal(Direction));
			ClueText = FText::Format(INVTEXT("An interesting point of interest with loot {0} of a {1}."), Cardinal, POI->DisplayName);
		}
		
		UE_LOG(LogFrette, Log, TEXT("Clue generated. Leads to landmark: %s, is near objective: %d"), *POI->GetName(), bIsPrimaryClue);
	}
	
	NumCluesFound++;
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
		NumCluesPlaced++;
	}
	
	require(NearLandmarks.Num() > 0, "No landmarks placed within the objective's radius. Primary clues won't be possible.");
	require(FarLandmarks.Num() > 0, "No landmarks placed outside the objective's radius. Secondary clues won't be possible.");
	
	const int32 TotalLandmarks = NearLandmarks.Num() + FarLandmarks.Num();
	require(NumCluesPlaced <= TotalLandmarks, "%d clues placed but only %d total landmarks exist.", NumCluesPlaced, TotalLandmarks);
}

bool AFretteGameMode::ShouldPickPrimaryClue() const
{
	if (NumPrimaryCluesFound >= NearLandmarks.Num())
	{
		UE_LOG(LogFrette, Log, TEXT("All primary clues found, must pick secondary clue."));
		return false;
	}
	
	const float PrimaryClueExpectation = PrimaryCluesRatioTarget * (NumCluesFound + 1);
	const float Deficit = PrimaryClueExpectation - NumPrimaryCluesFound;
	const float BaseProbability = FMath::Clamp(Deficit, 0.0f, 1.0f);
	const float Ramp = 1.0f - FMath::Exp(-NumCluesFound * PrimaryCluesRatioTargetRampSteepness);
	const float Probability = FMath::Clamp(BaseProbability * Ramp, 0.0f, 1.0f);
	
	UE_LOG(LogFrette, Log, TEXT("Probability of clue %d/%d being primary: %f (expectation: %f, deficit: %f, base: %f, ramp: %f)"), NumCluesFound + 1, NumCluesPlaced, Probability, PrimaryClueExpectation, Deficit, BaseProbability, Ramp);
	
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
		"north",
		"north-east",
		"east",
		"south-east",
		"south",
		"south-west",
		"west",
		"north-west"
	};

	return Names[SectorIdx];
}