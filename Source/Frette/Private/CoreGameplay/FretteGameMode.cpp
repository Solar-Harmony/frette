#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "Components/FretteGameplayStatics.h"
#include "CoreGameplay/FretteClue.h"
#include "CoreGameplay/FretteClueTemplateSet.h"
#include "CoreGameplay/FretteGameState.h"
#include "CoreGameplay/FretteLandmark.h"
#include "CoreGameplay/FretteMainObjective.h"
#include "Frette/Frette.h"

FText AFretteGameMode::GenerateClue(const AFrettePlayerCharacter* Interactor, const UFretteClueTemplateSet* Template)
{
	FFretteClueInfo Info;
	
	Info.ObjectiveName = MainObjective->DisplayName;
	
	const EClueType PickedType = bPlayerCollectedObjective
		? EClueType::Dud
		: PickNextClueType();
	
	if (PickedType == EClueType::Dud)
	{
		Info.Type = EClueType::Dud;
		NumDudCluesFound++;
		NumCluesFound++;
		return Template->GenerateClueText(Info);
	}
	
	const bool bIsPrimaryClue = PickedType == EClueType::MainObjective;
	const AFretteLandmark* POI = GetRandomLandmark(bIsPrimaryClue);
	if (POI == nullptr)
	{
		UE_LOG(LogFrette, Warning, TEXT("Failed to generate clue: no more landmarks available!"));
		return Template->GenerateClueText(Info);
	}
	
	Info.Type = bIsPrimaryClue ? EClueType::MainObjective : EClueType::PointOfInterest;
	Info.LandmarkName = POI->DisplayName;
	Info.LandmarkDescription = POI->Description;
	Info.LandmarkLoot = POI->ThingOfInterest;
		
	const FVector2D Direction((POI->GetActorLocation() - Interactor->GetActorLocation()).GetSafeNormal());
	const ECardinalDirection CardinalDirection = UFretteGameplayStatics::DirVectorToCardinal(Direction);
	
	// temporary of course
	static const FText Names[] =
	{
		INVTEXT("nord"),
		INVTEXT("nord-est"),
		INVTEXT("est"),
		INVTEXT("sud-est"),
		INVTEXT("sud"),
		INVTEXT("sud-ouest"),
		INVTEXT("ouest"),
		INVTEXT("nord-ouest")
	};

	Info.CardinalDirection = Names[static_cast<uint8>(CardinalDirection)];
	
	if (bIsPrimaryClue)
	{
		NumPrimaryCluesFound++;
	}
	NumCluesFound++;
		
	UE_LOG(LogFrette, Log, TEXT("Clue generated. Leads to landmark: %s, is near objective: %d"), *POI->GetName(), bIsPrimaryClue);
	return Template->GenerateClueText(Info);
}

void AFretteGameMode::ProbeForObjective(const AFrettePlayerCharacter* PlayerCharacter)
{
	const float Dist = FVector::Dist(PlayerCharacter->GetActorLocation(), MainObjective->GetActorLocation());
	if (Dist <= MainObjective->RightOnObjectiveRadiusCm)
	{
		bPlayerCollectedObjective = true;
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->AddItem(MainObjective->ObjectiveItemData);
	}
	else if (Dist <= MainObjective->NearObjectiveRadiusCm)
	{
		// TODO: Give the player some feedback that they are close
	}
}

void AFretteGameMode::CheckVictory(const AFrettePlayerCharacter* PlayerCharacter) const
{
	const UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
	const bool bHasTreasure = Inventory->HasItemOfType<UFretteObjectiveItem>();
	if (bHasTreasure)
	{
		AFretteGameState* GameState = GetGameState<AFretteGameState>();
		GameState->GameOutcome = EGameOutcome::Victory;
	}
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

EClueType AFretteGameMode::PickNextClueType() const
{
	// this exists to suppress the chance of getting a primary clue early-game
	const float Ramp = 1.0f - FMath::Exp(-NumCluesFound * ClueRatioRampSteepness);

	const float DudExpectation = DudClueRatioTarget * (NumCluesFound + 1);
	const float DudDeficit = fmax(0.0f, DudExpectation - NumDudCluesFound);
	const float DudProbability = DudDeficit / (NumCluesFound + 1);

	float Choice = FMath::FRand();

	if (Choice < DudProbability)
	{
		UE_LOG(LogFrette, Log, TEXT("Clue %d/%d -> Dud (P_dud=%f, ramp=%f, deficit=%f)"), NumCluesFound + 1, NumCluesPlaced, DudProbability, Ramp, DudDeficit);
		return EClueType::Dud;
	}

	if (NearLandmarks.IsEmpty())
	{
		UE_LOG(LogFrette, Log, TEXT("Clue %d/%d -> Secondary (all primary landmarks exhausted)"), NumCluesFound + 1, NumCluesPlaced);
		return EClueType::PointOfInterest;
	}

	if (FarLandmarks.IsEmpty())
	{
		UE_LOG(LogFrette, Log, TEXT("Clue %d/%d -> Primary (all secondary landmarks exhausted)"), NumCluesFound + 1, NumCluesPlaced);
		return EClueType::MainObjective;
	}

	const float PrimaryExpectation = PrimaryCluesRatioTarget * (NumCluesFound + 1) * Ramp;
	const float PrimaryDeficit = fmax(0.0f, PrimaryExpectation - NumPrimaryCluesFound);
	const float PrimaryProbability = PrimaryDeficit / (NumCluesFound + 1);

	// This is done implicitly by the checks, but needed for logs
	const float SecondaryProbability = 1.0f - DudProbability - PrimaryProbability;

	Choice -= DudProbability;

	if (Choice < PrimaryProbability)
	{
		UE_LOG(LogFrette, Log, TEXT("Clue %d/%d -> Primary (P_primary=%f, P_dud=%f, P_Secondary=%f, ramp=%f)"), NumCluesFound + 1, NumCluesPlaced, PrimaryProbability, DudProbability, SecondaryProbability, Ramp);
		return EClueType::MainObjective;
	}
	
	UE_LOG(LogFrette, Log, TEXT("Clue %d/%d -> Secondary (P_primary=%f, P_dud=%f, P_Secondary=%f, ramp=%f)"), NumCluesFound + 1, NumCluesPlaced, PrimaryProbability, DudProbability, SecondaryProbability, Ramp);
	return EClueType::PointOfInterest;
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
