#pragma once

#include "CoreMinimal.h"
#include "FretteBodyPartData.h"
#include "Character/FretteBaseCharacter.h"
#include "UObject/Object.h"
#include "FretteBodyPartInstance.generated.h"

struct FGameplayTag;
class UFretteBodyPartData;
/**
 * 
 */
UCLASS()
class FRETTE_API UFretteBodyPartInstance : public UObject
{
	GENERATED_BODY()

public:
	FGameplayTag GetAssociatedTag() const { return SourceData->BodyPartTag; }
	void Initialize(UFretteBodyPartData* InSourceData, AFretteBaseCharacter* Owner);
	void ApplyDamage(float Damage, FGameplayTag DamageType);
	bool IsTriggered(const FBodyPartEffectRule& Rule, float Damage, FGameplayTag DamageType) const;
	void ApplyEffect(const FBodyPartEffectRule& Rule) const;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, float> AccumulatedDamageByType;

private:
	UPROPERTY()
	TObjectPtr<UFretteBodyPartData> SourceData;

	UPROPERTY()
	TObjectPtr<AFretteBaseCharacter> OwnerCharacter;
};