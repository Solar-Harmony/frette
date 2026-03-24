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
	UFretteBodyPartInstance* FindBodyPart(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag);

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Part")
	void AddValueFromBodyPartTag(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag,
		int Value,
		UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType);
	void AddValueFromBoneName(FName BoneName, int Value, FGameplayTag ValueType);

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Part")
	void AddValueToAllParts(int Value, UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType);

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