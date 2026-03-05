#include "CoreGameplay/FretteClue.h"

#include "CoreGameplay/FretteGameMode.h"
#include "CoreGameplay/FretteLandmark.h"
#include "Kismet/GameplayStatics.h"

void AFretteClue::OnPickUp_Implementation()
{
	AFretteGameMode* GameMode = CastChecked<AFretteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	FText ClueText;
	
	if (FMath::FRand() < DudClueChance)
	{
		ClueText = INVTEXT("This is some fascinating lore.");
		GameMode->OnClueActivated.Broadcast(ClueText);
		return;
	}
	
	const bool bIsPrimaryClue = ShouldGivePrimaryClue(GameMode->GetNumCluesFound(), GameMode->GetNumCluesMax());
	const AFretteLandmark* POI = GameMode->GetRandomLandmark(bIsPrimaryClue);
	
	if (bIsPrimaryClue)
	{
		ClueText = FText::Format(INVTEXT("The main objective is near a %s."), POI->DisplayName);
	}
	else
	{
		ClueText = FText::Format(INVTEXT("An interesting point of interest with loot is near a %s."), POI->DisplayName);
	}
	
	GameMode->OnClueActivated.Broadcast(ClueText);
}

bool AFretteClue::ShouldGivePrimaryClue(int NumCluesFound, int NumCluesTotal) const
{
	const float NormalizedClueCount = static_cast<float>(NumCluesFound) / NumCluesTotal;
	
	auto Sigmoid = [this](float N) 
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
