#include "CoreGameplay/FretteGameMode.h"

#include "EngineUtils.h"
#include "CoreGameplay/FretteMainObjective.h"
#include "CoreGameplay/FrettePOI.h"

void AFretteGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AFretteMainObjective> It(GetWorld()); It; ++It)
	{
		MainObjective = *It;
		break;
	}
	
	const float NearObjectiveRadiusSq = FMath::Square(MainObjective->NearObjectiveRadiusCm);
	for (TActorIterator<AFrettePOI> It(GetWorld()); It; ++It)
	{
		AFrettePOI* POI = *It;
		const float DistanceToObjectiveSq = FVector::DistSquared(POI->GetActorLocation(), MainObjective->GetActorLocation());
		if (DistanceToObjectiveSq <= NearObjectiveRadiusSq)
		{
			POI->bIsPrimaryPOI = true;
			LocationsNearObjective.Add(POI);
		}
	}
}