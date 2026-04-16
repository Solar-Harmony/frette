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
struct FFretteBodyPartValueTypeConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Meta = (Categories = "Frette.BodyPartValues"))
	FGameplayTag Type;

	// Whether this body part is essential to this value type
	UPROPERTY(EditAnywhere)
	bool bIsCritical = false;
	
	UPROPERTY(EditAnywhere)
	int MinValue = 0;
	
	UPROPERTY(EditAnywhere)
	int StartValue = 100;
	
	UPROPERTY(EditAnywhere)
	int MaxValue = 100;
	
	UPROPERTY(EditAnywhere, Category="Feedback")
	int FeedbackLowValue = 10;
	
	UPROPERTY(EditAnywhere, Category="Feedback")
	int FeedbackHighValue = 50;
};

UCLASS()
class UFretteBodyPartData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Meta = (Categories = "Frette.BodyPart"))
	FGameplayTag BodyPartTag;

	UPROPERTY(EditAnywhere)
	TArray<FFretteBodyPartValueTypeConfig> ValueDatas;

	UPROPERTY(EditAnywhere)
	TArray<FFretteEffectRuleEntry> EffectRules;
	
	const FFretteBodyPartValueTypeConfig* GetValueTypeConfig(FGameplayTag ValueType) const
	{
		return ValueDatas.FindByPredicate([&](const FFretteBodyPartValueTypeConfig& Config)
		{
			return Config.Type == ValueType;
		});
	}

	int GetMinValueForType(FGameplayTag Type) const
	{
		const auto* Config = GetValueTypeConfig(Type);
		return Config ? Config->MinValue : 0;
	}
	
	int GetStartValueForType(FGameplayTag Type) const
	{
		const auto* Config = GetValueTypeConfig(Type);
		return Config ? Config->StartValue : 100;
	};
	
	int GetMaxValueForType(FGameplayTag Type) const
	{
		const auto* Config = GetValueTypeConfig(Type);
		return Config ? Config->MaxValue : 100;
	}
	
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override
	{
		Super::PostEditChangeProperty(Event);

		for (FFretteBodyPartValueTypeConfig& Config : ValueDatas)
		{
			Config.FeedbackLowValue = FMath::Clamp(Config.FeedbackLowValue, Config.MinValue, Config.FeedbackHighValue);
			Config.FeedbackHighValue = FMath::Clamp(Config.FeedbackHighValue, Config.FeedbackLowValue, Config.MaxValue);
		}
	}
	#endif
};