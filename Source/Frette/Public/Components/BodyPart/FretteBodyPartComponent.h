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

	UFUNCTION(BlueprintCallable)
	void ApplyDamageFromHit(const FName BoneName, float Damage);
	void ApplyDamageFromHit(FGameplayTag BodyPartTag, float Damage);
	void AddStatusEffectStackFromHit(FGameplayTag BodyPartTag, int StackAmount, FGameplayTag EffectTag);

	UFUNCTION(BlueprintPure)
	float GetBodyPartHealth(FGameplayTag BodyPartTag);
	UFretteBodyPartInstance* FindBodyPart(FGameplayTag BodyPartTag);
	void AddStatusEffectStackToAllParts(int StackAmount, FGameplayTag EffectTag);

	UFUNCTION()
	void OnRep_BodyParts();

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(FGameplayTag BodyPartTag, float Damage);

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