#pragma once

#include "CoreMinimal.h"
#include "FretteClueTemplateSet.generated.h"

enum class EClueType
{
	Dud,
	PointOfInterest,
	MainObjective,
};

struct FFretteClueInfo
{
	EClueType Type;
	FText LandmarkName = INVTEXT("ERR_LANDMARK_NAME_UNSET");
	FText LandmarkDescription = INVTEXT("ERR_LANDMARK_DESC_UNSET");
	FText ObjectiveName = INVTEXT("ERR_OBJECTIVE_NAME_UNSET");
	FText LandmarkLoot = INVTEXT("ERR_LANDMARK_LOOT_UNSET");
	FText CardinalDirection = INVTEXT("ERR_CARDINAL_DIRECTION_UNSET");
};

// Represents a medium, like a personal letter, business letter etc.
// available variables:
// {Objective} - name of the main objective
// {POI} - landmark name (e.g. "campement")
// {POI_Desc} - description of the landmark (e.g. "vestige d'un temps passé")
// {POI_Loot} - something that is at the landmark (e.g. "armes", "coffre barré", "plein de nourriture en conserve")
// {Dir} - cardinal direction from the clue to the landmark (e.g. "nord-ouest")
UCLASS(BlueprintType)
class UFretteClueTemplateSet : public UDataAsset
{
	GENERATED_BODY()

public:
	// For dud clues. Cannot use any info!
	UPROPERTY(EditDefaultsOnly, meta = (MultiLine = true))
	TArray<FText> LoreClueTemplates;
	
	// For the main objective. Cannot use POI loot info.
	UPROPERTY(EditDefaultsOnly, meta = (MultiLine = true))
	TArray<FText> PrimaryClueTemplates;
	
	// For the landmarks with random loot
	UPROPERTY(EditDefaultsOnly, meta = (MultiLine = true))
	TArray<FText> SecondaryClueTemplates;
	
	FText GenerateClueText(const FFretteClueInfo& Info) const
	{
		FText Template;
		
		if (Info.Type == EClueType::Dud)
			Template = PickRandom(LoreClueTemplates);
		else if (Info.Type == EClueType::MainObjective)
			Template = PickRandom(PrimaryClueTemplates);
		else
			Template = PickRandom(SecondaryClueTemplates);
		
		FFormatNamedArguments Args;
		Args.Add(TEXT("POI"), Info.LandmarkName);
		Args.Add(TEXT("POI_Desc"), Info.LandmarkDescription);
		Args.Add(TEXT("Objective"), Info.ObjectiveName);
		Args.Add(TEXT("POI_Loot"), Info.LandmarkLoot);
		Args.Add(TEXT("Dir"), Info.CardinalDirection);
		
		return FText::Format(Template, Args);
	}

	static FText PickRandom(const TArray<FText>& Templates)
	{
		if (Templates.Num() == 0)
			return INVTEXT("<NO TEMPLATE AVAILABLE>");
		
		const int32 Index = FMath::RandRange(0, Templates.Num() - 1);
		return Templates[Index];
	}
};