#pragma once

#include "CoreMinimal.h"
#include "FretteBonesToTagData.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/BodyPart/FretteBodyPartInstance.h"
#include "FretteBodyPartComponent.generated.h"

class UFretteBodyPartData;

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteBodyPartComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteBodyPartComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFretteBodyPartInstance* FindBodyPart(FGameplayTag BodyPartTag);

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Part")
	void AddValueFromHit(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag,
		int Value,
		UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag EffectTag);

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Part")
	void AddValueToAllParts(int Value, FGameplayTag EffectTag);

	UFUNCTION(BlueprintPure)
	float GetBodyPartHealth(FGameplayTag BodyPartTag);

	UFUNCTION()
	void OnRep_BodyParts();

	FGameplayTag GetBodyPartFromBoneName(FName BoneName) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TArray<TObjectPtr<UFretteBodyPartData>> BodyPartData;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TObjectPtr<UFretteBonesToTagData> BoneTagDataAsset;

	UPROPERTY(ReplicatedUsing = OnRep_BodyParts, BlueprintReadOnly)
	TArray<TObjectPtr<UFretteBodyPartInstance>> BodyPartInstances;

};