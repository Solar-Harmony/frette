

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FretteInputConfig.generated.h"


USTRUCT(BlueprintType)
struct FFretteInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly,Meta = (Categories = "InputTag"))
	FGameplayTag InputTag = FGameplayTag();
};

UCLASS()
class FRETTE_API UFretteInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FFretteInputAction> AbilityInputActions;
};
