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
	
	FFretteBodyPartChangeEvent() = default;
	
	FFretteBodyPartChangeEvent(FGameplayTag InBodyPartTag, FGameplayTag InValueTag, float InNewValue, float InValueDelta)
		: BodyPartTag(InBodyPartTag)
		, ValueTypeTag(InValueTag)
		, NewValue(InNewValue)
		, ValueDelta(InValueDelta) {}

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag BodyPartTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ValueTypeTag;
	
	UPROPERTY(BlueprintReadOnly)
	float NewValue = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	float ValueDelta = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBodyPartValueChanged, const FFretteBodyPartChangeEvent&, ChangeEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBodyPartsInitialized);

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteBodyPartComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteBodyPartComponent();
	virtual void BeginPlay() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool Ciboire_DeleteMoiPasCalisse() const;
	
	UFretteBodyPartInstance* FindBodyPart(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag) const;

	UFUNCTION(BlueprintPure, Category = "Frette|Body Parts")
	float GetValueFromBodyPart(
		UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag,
		UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueTypeTag) const;

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Parts")
	void AddValueFromBodyPartTag(UPARAM(meta = (Categories = "Frette.BodyPart")) FGameplayTag BodyPartTag,
		float Value,
		UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType);

	void AddValueFromBoneName(FName BoneName, float Value, FGameplayTag ValueType);

	UFUNCTION(BlueprintCallable, Category = "Frette|Body Parts")
	void AddValueToAllParts(float Value, UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueType);
	
	UFretteBodyPartInstance* GetInstanceFromBodyPartTag(FGameplayTag BodyPartTag); 
	
	UFUNCTION(BlueprintPure, Category = "Frette|Body Parts")
	float GetNormalizedCriticalValue(UPARAM(meta = (Categories = "Frette.BodyPartValues")) FGameplayTag ValueTag, bool bForFeedback) const;
	void ResetValueForAllBodyParts(FGameplayTag ValueType);

	UFUNCTION(BlueprintPure, Category = "Frette|Body Parts")
	bool AreBodyPartsInitialized() const { return BodyPartInstances.Num() > 0; }
	
	UFUNCTION()
	void OnRep_BodyPartInstances();
	
	UFUNCTION(Client, Reliable)
	void Client_NotifyBodyPartChange(const FFretteBodyPartChangeEvent& ChangeEvent);

	UFUNCTION(BlueprintPure)
	FGameplayTag GetBodyPartFromBoneName(FName BoneName) const;
	
	bool GetRepresentativeBoneForTag(FGameplayTag BodyPartTag, FName& OutBoneName) const;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TArray<TObjectPtr<UFretteBodyPartData>> BodyPartData;
	
	UPROPERTY(BlueprintAssignable, Category = "Frette|Body Parts")
	FOnBodyPartValueChanged OnBodyPartValueChanged;

	UPROPERTY(BlueprintAssignable, Category = "Frette|Body Parts")
	FOnBodyPartsInitialized OnBodyPartsInitialized;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TObjectPtr<UFretteBonesToTagData> BoneTagDataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TMap<FGameplayTag, FGameplayTagContainer> BoneTagNeighbours;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_BodyPartInstances, BlueprintReadOnly)
	TArray<TObjectPtr<UFretteBodyPartInstance>> BodyPartInstances;
	
private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UFretteBodyPartInstance>> BodyPartTagToInstanceMap;

	UPROPERTY()
	TMap<FGameplayTag, FName> BodyPartTagToRepresentativeBoneMap;
};
