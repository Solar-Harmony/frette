#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FretteAbilitySystemComponent.generated.h"

class UFretteAbilitySetDataAsset;
USTRUCT()
struct FFretteGameplayEffectConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta=(AllowAbstract = false))
	TSubclassOf<UGameplayEffect> EffectClass;
	
	UPROPERTY(EditAnywhere)
	float Level = 1.0f;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> Magnitudes;
};

USTRUCT(BlueprintType)
struct FFretteGameplayAbilityConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowAbstract = false))
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;
	
	bool IsValid() const
	{
		return Ability != nullptr && InputTag.IsValid();
	}
};

UCLASS()
class FRETTE_API UFretteAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	static UFretteAbilitySystemComponent* Get(class AFretteBaseCharacter* Character);
	
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
	
	void GrantAbility(const FFretteGameplayAbilityConfig& AbilityConfig, UObject* SourceObject);
	void GrantAbilities(const TArray<FFretteGameplayAbilityConfig>& AbilityConfigs, UObject* SourceObject);
	void GrantAbilities(const UFretteAbilitySetDataAsset* AbilitySet, UObject* SourceObject);
	
	void RevokeAbility(const FFretteGameplayAbilityConfig& AbilityConfig);
	void RevokeAbilities(const TArray<FFretteGameplayAbilityConfig>& AbilityConfigs);
	void RevokeAbilities(const UFretteAbilitySetDataAsset* AbilitySet);
	
	FActiveGameplayEffectHandle ApplyEffect(const FFretteGameplayEffectConfig& Config, const UObject* SourceObject);
	
protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	UPROPERTY()
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> InputAbilityMap;
};