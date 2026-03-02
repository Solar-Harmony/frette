#include "CoreGameplay/FrettePOISubsystem.h"

#include "CoreGameplay/FrettePOI.h"
#include "Kismet/GameplayStatics.h"

void UFrettePOISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TArray<AActor*> FoundPOIs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFrettePOI::StaticClass(), FoundPOIs);
	
	for (AActor* Actor : FoundPOIs) 
	{
		AFrettePOI* POI = CastChecked<AFrettePOI>(Actor);
		ClassToPOIs.AddUnique(POI->GetClass(), POI);
	}
}

TArray<AFrettePOI*> UFrettePOISubsystem::GetPOIsByClass(TSubclassOf<AFrettePOI> POIClass) const
{
	TArray<AFrettePOI*> Result;
	ClassToPOIs.MultiFind(POIClass, Result);
	return Result;
}

TSubclassOf<AFrettePOI> UFrettePOISubsystem::GetRandomPOIClass() const
{
	const int32 Idx = FMath::RandRange(0, ClassToPOIs.Num() - 1);
	TArray<TSubclassOf<AFrettePOI>> Keys;
	ClassToPOIs.GetKeys(Keys);
	return Keys[Idx];
}