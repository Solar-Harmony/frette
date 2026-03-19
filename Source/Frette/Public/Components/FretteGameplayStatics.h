#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FretteGameplayStatics.generated.h"

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest,
};

UCLASS()
class FRETTE_API UFretteGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static ECardinalDirection DirVectorToCardinal(const FVector2D& Dir);
};

