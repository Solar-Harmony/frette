#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "Character/FretteNotificationsComponent.h"
#include "Components/FretteGameplayStatics.h"
#include "CoreGameplay/FretteClue.h"
#include "CoreGameplay/FretteClueTemplateSet.h"
#include "CoreGameplay/FretteGameState.h"
#include "CoreGameplay/FretteLandmark.h"
#include "CoreGameplay/FretteMainObjective.h"
#include "Frette/Frette.h"

void AFretteGameMode::UpdateTimeBeforeNextPrimaryClue()
{
	const float TimeSinceGameStarted = GetWorld()->GetTimeSeconds() - GameStartTime;
	const float DesiredRemainingGameTime = Cfg->DesiredGameDuration - TimeSinceGameStarted;
	const float TimePerPrimaryClue = DesiredRemainingGameTime / NearLandmarks.Num();
	MaxTimeBeforeNextPrimaryClue = TimePerPrimaryClue
		+ FMath::RandRange(0.f, Cfg->TimeUncertainty * TimePerPrimaryClue);
	
	// Let's make the min time a fraction of the max time so they scale together
	const float MinTimePerPrimaryClue = Cfg->MinTimeRatio * TimePerPrimaryClue;
	MinTimeBeforeNextPrimaryClue = MinTimePerPrimaryClue
		+ FMath::RandRange(0.f, Cfg->TimeUncertainty * MinTimePerPrimaryClue);
}

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
	else if (PickedType == EClueType::MainObjective)
	{
		LastPrimaryClueFoundTime = GetWorld()->GetTimeSeconds();
		UpdateTimeBeforeNextPrimaryClue();
	}

	const bool bIsPrimaryClue = PickedType == EClueType::MainObjective;
	const AFretteLandmark* POI = GetRandomLandmark(bIsPrimaryClue);
	if (POI == nullptr)
	{
		UE_LOG(LogFrette, Warning, TEXT("Failed to generate clue: no more landmarks available!"));
		return Template->GenerateClueText(Info);
	}

	Info.Type = bIsPrimaryClue ? EClueType::MainObjective : EClueType::PointOfInterest;
	Info.LandmarkName = UFretteClueTemplateSet::PickRandom(POI->DisplayNames);
	Info.LandmarkDescription = UFretteClueTemplateSet::PickRandom(POI->Descriptions);
	Info.LandmarkLoot = INVTEXT("RIEN PANTOUTE (not implemented)");

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
	if (bPlayerCollectedObjective)
	{
		UFretteNotificationsComponent::Notify(PlayerCharacter, INVTEXT("You have already collected the objective! Go to the extract point."));
		return;
	}

	const float Dist = FVector::Dist(PlayerCharacter->GetActorLocation(), MainObjective->GetActorLocation());
	if (Dist <= MainObjective->RightOnObjectiveRadiusCm)
	{
		bPlayerCollectedObjective = true;
		UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
		Inventory->AddItem(MainObjective->ObjectiveItemData);
		MainObjective->SetCollected(true);
		UFretteNotificationsComponent::NotifyAll(PlayerCharacter, INVTEXT("YOU HAVE FOUND THE OBJECTIVE! BRING IT BACK TO THE EXTRACTION POINT TO WIN!"));
	}
	else if (Dist <= MainObjective->NearObjectiveRadiusCm)
	{
		UFretteNotificationsComponent::Notify(PlayerCharacter, INVTEXT("You are near the objective! Search around to find it."));
	}
	else
	{
		UFretteNotificationsComponent::Notify(PlayerCharacter, INVTEXT("You dig the earth around, but find nothing."));
	}
}

void AFretteGameMode::CheckVictory(const AFrettePlayerCharacter* PlayerCharacter) const
{
	const UFretteInventoryComponent* Inventory = PlayerCharacter->GetPlayerInventory();
	const bool bHasTreasure = Inventory->HasItemOfClass<UFretteObjectiveItem>();
	if (bHasTreasure)
	{
		AFretteGameState* State = GetGameState<AFretteGameState>();
		State->GameOutcome = EGameOutcome::Victory;
		UFretteNotificationsComponent::NotifyAll(PlayerCharacter, INVTEXT("GG YOU WON"));
	}
}

bool AFretteGameMode::IsGameEnded() const
{
	return GetGameState<AFretteGameState>()->GameOutcome != EGameOutcome::InProgress;
}

void AFretteGameMode::BeginPlay()
{
	Super::BeginPlay();
	LastPrimaryClueFoundTime = GetWorld()->GetTimeSeconds();
	GameStartTime = GetWorld()->GetTimeSeconds();

	for (TActorIterator<AFretteMainObjective> It(GetWorld()); It; ++It)
	{
		precondition(MainObjective == nullptr, "Multiple main objectives found in the level! There should be only one.");
		MainObjective = *It;
	}

	precondition(IsValid(MainObjective), "No main objective found in the level! Make sure to place one in the level.");

	const FVector2D ObjectiveLocation2D(MainObjective->GetActorLocation());
	const float NearObjectiveRadiusSq = FMath::Square(MainObjective->NearObjectiveRadiusCm);

	for (TActorIterator<AFretteLandmark> It(GetWorld()); It; ++It)
	{
		AFretteLandmark* Landmark = *It;
		
		// exclude generic landmarks like houses
		// TODO: this breaks SRP and we should use a self-registrating component
		if (!Landmark->bUsedByClueSystem)
			continue;

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

	precondition(NearLandmarks.Num() > 0, "No landmarks placed within the objective's radius. Primary clues won't be possible.");
	precondition(FarLandmarks.Num() > 0, "No landmarks placed outside the objective's radius. Secondary clues won't be possible.");

	const int32 TotalLandmarks = NearLandmarks.Num() + FarLandmarks.Num();
	precondition(NumCluesPlaced >= TotalLandmarks, "There are %d landmarks in the level but only %d clues were placed.", TotalLandmarks, NumCluesPlaced);
	
	UpdateTimeBeforeNextPrimaryClue();
}

EClueType AFretteGameMode::PickNextClueType() const
{
	const float TimeSinceGameStart = GetWorld()->GetTimeSeconds() - GameStartTime;
	
	// this exists to suppress the chance of getting a primary clue early-game
	const float Ramp = FMath::Min(1.f, FMath::Pow(TimeSinceGameStart / (Cfg->DesiredGameDuration * 0.6), Cfg->RampDegree));
	
	const float TimeSinceLastPrimary = GetWorld()->GetTimeSeconds() - LastPrimaryClueFoundTime;
	const float TimeForce = FMath::Clamp(
		(TimeSinceLastPrimary - MinTimeBeforeNextPrimaryClue) / (MaxTimeBeforeNextPrimaryClue - MinTimeBeforeNextPrimaryClue),
		0.f, 1.f);
	// Do we want to reduce the force if primary clues have already been found?
	
	const float DudExpectation = Cfg->DudClueRatioTarget * (NumCluesFound + 1);
	const float DudDeficit = FMath::Max(0.0f, DudExpectation - NumDudCluesFound);
	float DudProbability = DudDeficit / (NumCluesFound + 1);
	DudProbability = FMath::Lerp(DudProbability, 0.f, TimeForce);
	
	UE_LOG(LogFrette, Log, TEXT("============================================= Clue gen ==========================================="));
	UE_LOG(LogFrette, Log, TEXT("Clue gen: NumCluesFound[%d] NumPrimaryCluesFound[%d] NumDudCluesFound[%d]"), NumCluesFound, NumPrimaryCluesFound, NumDudCluesFound);
	UE_LOG(LogFrette, Log, TEXT("Clue gen: Ramp[%f] GameTime[%f/%f] TimeSinceLastPrimary[%f] MinTimeBeforeNextPrimaryClue[%f] MaxTimeBeforeNextPrimaryClue[%f] TimeForce[%f]"),
		Ramp, TimeSinceGameStart, Cfg->DesiredGameDuration, TimeSinceLastPrimary, MinTimeBeforeNextPrimaryClue, MaxTimeBeforeNextPrimaryClue, TimeForce);
	UE_LOG(LogFrette, Log, TEXT("Clue gen: DudDeficit[%f] DudProbability[%f]"), DudDeficit, DudProbability);

	float Choice = FMath::FRand();

	if (Choice < DudProbability)
	{
		UE_LOG(LogFrette, Log, TEXT("Clue gen: Clue %d/%d -> Dud"), NumCluesFound + 1, NumCluesPlaced);
		return EClueType::Dud;
	}
	
	if (NearLandmarks.IsEmpty())
	{
		UE_LOG(LogFrette, Log, TEXT("Clue gen: Clue %d/%d -> Secondary (all primary landmarks exhausted)"), NumCluesFound + 1, NumCluesPlaced);
		return EClueType::PointOfInterest;
	}

	if (FarLandmarks.IsEmpty())
	{
		UE_LOG(LogFrette, Log, TEXT("Clue gen: Clue %d/%d -> Primary (all secondary landmarks exhausted)"), NumCluesFound + 1, NumCluesPlaced);
		return EClueType::MainObjective;
	}
	
	const float PrimaryExpectation = Cfg->PrimaryCluesRatioTarget * (NumCluesFound + 1);
	const float PrimaryDeficit = FMath::Max(0.0f, PrimaryExpectation - NumPrimaryCluesFound);
	float PrimaryProbability = PrimaryDeficit / (NumCluesFound + 1);
	// We do not make it strictly impossible to get a primary clue when the time force is 0 to make luckiness possible (0.05)
	PrimaryProbability = FMath::Lerp(
		FMath::Lerp(Cfg->MinPrimaryProb, PrimaryProbability, TimeForce),
		1.0, TimeForce) * Ramp;
	PrimaryProbability = FMath::Min(PrimaryProbability, 1.0f - DudProbability);
	
	// The lerping is done a bit differently for the dud and primary probs so let's scale things
	const float Total = DudProbability + PrimaryProbability + Cfg->MinSecondaryProb;
	if (Total > 1.0f)
	{
		PrimaryProbability /= Total;
		DudProbability /= Total;
	}

	// This is done implicitly by the checks, but needed for logs
	const float SecondaryProbability = 1.0f - DudProbability - PrimaryProbability;
	
	UE_LOG(LogFrette, Log, TEXT("Clue gen: PrimaryDeficit[%f] PrimaryProbability[%f]"), PrimaryDeficit, PrimaryProbability);
	UE_LOG(LogFrette, Log, TEXT("Clue gen: Secondary[%f]"), SecondaryProbability);

	Choice -= DudProbability;

	if (Choice < PrimaryProbability)
	{
		UE_LOG(LogFrette, Log, TEXT("Clue gen: Clue %d/%d -> Primary"), NumCluesFound + 1, NumCluesPlaced);
		return EClueType::MainObjective;
	}

	UE_LOG(LogFrette, Log, TEXT("Clue gen: Clue %d/%d -> Secondary"), NumCluesFound + 1, NumCluesPlaced);
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

//Risque d'avoir des problemes si jamais on détruit et recréer des pawns
void AFretteGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	AFrettePlayerCharacter* PlayerCharacter = Cast<AFrettePlayerCharacter>(NewPlayer->GetPawn());

	if (PlayerCharacter)
	{
		Players.Add(PlayerCharacter);
		PlayerCharacter->OnPlayerDied.AddDynamic(this, &AFretteGameMode::OnPlayerDied);
	}
}

void AFretteGameMode::OnPlayerDied(AFrettePlayerCharacter* PlayerCharacter)
{
	if (GetIfAllPlayersDead())
	{
		AFretteGameState* State = GetGameState<AFretteGameState>();
		State->GameOutcome = EGameOutcome::Defeat;
		UFretteNotificationsComponent::NotifyAll(PlayerCharacter, INVTEXT("YOU LOST"));
	}
}

bool AFretteGameMode::GetIfAllPlayersDead() const
{
	for (const auto& Player : Players)
	{
		if (!Player->GetIsDead())
			return false;
	}

	return true;
}