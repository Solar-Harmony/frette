#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FretteAbilitySetDataAsset.generated.h"

struct FFretteGameplayAbilityConfig;
class UGameplayAbility;

UCLASS()
class FRETTE_API UFretteAbilitySetDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FFretteGameplayAbilityConfig> AbilityConfigs;
};

USTRUCT()
struct FRETTE_API FFretteAbilitySet
{
	GENERATED_BODY()

public:
	TArray<FFretteGameplayAbilityConfig> GetAbilityConfigs() const
	{
		if (bUseAsset && AbilitySetAsset)
		{
			return AbilitySetAsset->AbilityConfigs;
		}
		
		return AbilityConfigs;
	}
	
protected:
	UPROPERTY(EditDefaultsOnly)
	bool bUseAsset = false;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseAsset", EditConditionHides))
	TObjectPtr<UFretteAbilitySetDataAsset> AbilitySetAsset;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "!bUseAsset", EditConditionHides))
	TArray<FFretteGameplayAbilityConfig> AbilityConfigs;
};