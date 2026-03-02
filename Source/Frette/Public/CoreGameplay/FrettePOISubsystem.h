#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(Transient)
	TMultiMap<TSubclassOf<AFrettePOI>, AFrettePOI*> ClassToPOIs;
};