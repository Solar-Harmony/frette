#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FretteBodyPartData.generated.h"

class UFretteBodyPartEffectRule;
class UGameplayEffect;
class UGameplayAbility;

//Je sais pas trop si je devrais juste avoir les effect a déclancher directement dans la regle
USTRUCT(BlueprintType)
struct FFretteEffectRuleEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UFretteBodyPartEffectRule> Rule = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
};

USTRUCT(BlueprintType)
struct FFretteValueData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Meta = (Categories = "Frette.BodyPartValues"))
	FGameplayTag Type;

	UPROPERTY(EditAnywhere)
	int MaxValue = 100.f;

	UPROPERTY(EditAnywhere)
	int MinValue = 0.f;

	UPROPERTY(EditAnywhere)
	int StartValue = 100.f;
};

UCLASS()
class UFretteBodyPartData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Meta = (Categories = "Frette.BodyPart"))
	FGameplayTag BodyPartTag;

	UPROPERTY(EditAnywhere)
	TArray<FFretteValueData> ValueDatas;

	UPROPERTY(EditAnywhere)
	TArray<FFretteEffectRuleEntry> EffectRules;

	int GetMaxValueForType(FGameplayTag Type) const
	{
		for (const FFretteValueData& ValueData : ValueDatas)
		{
			if (Type == ValueData.Type)
				return ValueData.MaxValue;
		}
		return 100;
	};

	int GetMinValueForType(FGameplayTag Type) const
	{
		for (const FFretteValueData& ValueData : ValueDatas)
		{
			if (Type == ValueData.Type)
				return ValueData.MinValue;
		}

		return 0;
	};

	int GetStartValueForType(FGameplayTag Type) const
	{
		for (const FFretteValueData& ValueData : ValueDatas)
		{
			if (Type == ValueData.Type)
				return ValueData.StartValue;
		}
		return 100;
	};

};