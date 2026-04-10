#pragma once

#include "CoreMinimal.h"
#include "FretteBonesToTagData.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/BodyPart/FretteBodyPartInstance.h"
#include "FretteBodyPartComponent.generated.h"

class UFretteBodyPartData;

USTRUCT(BlueprintType)
struct FFretteBodyPartChangeEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag BodyPartTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ValueTypeTag;
	
	UPROPERTY(BlueprintReadOnly)
	int NewValue;
	
	UPROPERTY(BlueprintReadOnly)
	int ValueDelta;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBodyPartValueChanged, const FFretteBodyPartChangeEvent&, ChangeEvent);

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteBodyPartComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteBodyPartComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFretteBodyPartInstance* FindBodyPart(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag) const;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	int GetValueFromBodyPart(UPARAM(meta = (Categories = "Frette.BodyPart"))FGameplayTag BodyPartTag,
		UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType) const;

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Part")
	void AddValueFromBodyPartTag(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag,
		int Value,
		UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType);

	void AddValueFromBoneName(FName BoneName, int Value, FGameplayTag ValueType);

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Part")
	void AddValueToAllParts(int Value, UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType);
	
	// Retrieves the value specified attribute for either the Head or Torso, whichever is lowest.
	UFUNCTION(BlueprintPure, Category = "Frette|Body Part")
	float GetNormalizedCriticalValue(UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueTag) const;

	UFUNCTION()
	void OnRep_BodyParts();
	
	UFUNCTION(Client, Reliable)
	void Client_NotifyBodyPartChange(const FFretteBodyPartChangeEvent& ChangeEvent);

	FGameplayTag GetBodyPartFromBoneName(FName BoneName) const;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TArray<TObjectPtr<UFretteBodyPartData>> BodyPartData;
	
	UPROPERTY(BlueprintAssignable, Category = "Frette|Body Part")
	FOnBodyPartValueChanged OnBodyPartValueChanged;

protected:
	virtual void ReadyForReplication() override;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TObjectPtr<UFretteBonesToTagData> BoneTagDataAsset;

	UPROPERTY(ReplicatedUsing = OnRep_BodyParts, BlueprintReadOnly)
	TArray<TObjectPtr<UFretteBodyPartInstance>> BodyPartInstances;

};