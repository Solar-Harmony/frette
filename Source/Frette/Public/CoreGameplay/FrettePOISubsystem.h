#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "FrettePOISubsystem.generated.h"

class AFrettePOI;

/**
 * Manager for caching the POIs in the level.
 */
UCLASS()
class UFrettePOISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category = "Frette")
	TArray<AFrettePOI*> GetPOIsByClass(TSubclassOf<AFrettePOI> POIClass) const;
	
	UFUNCTION(BlueprintCallable, Category = "Frette")
	TSubclassOf<AFrettePOI> GetRandomPOIClass() const;
	
private:
	// uproperty doesn't support multimap
	// doesn't matter because POIs are added once at start and never removed, so their lifetime = this subsystem's lifetime 
	TMultiMap<TSubclassOf<AFrettePOI>, AFrettePOI*> ClassToPOIs;
};