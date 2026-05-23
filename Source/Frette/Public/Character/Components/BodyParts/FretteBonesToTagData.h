#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FretteBonesToTagData.generated.h"

/**
 * 
 */
UCLASS()
class FRETTE_API UFretteBonesToTagData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Frette.BodyPart"))
	TMap<FName, FGameplayTag> BoneToBodyPartTag;
};