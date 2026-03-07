#pragma once

#include "CoreMinimal.h"
#include "FretteBonesToTagData.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FretteBodyPartComponent.generated.h"

USTRUCT(BlueprintType)
struct FBodyPartHealthData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag BodyPartTag;

	UPROPERTY(BlueprintReadWrite)
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;

};

UCLASS(meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteBodyPartComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFretteBodyPartComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void ApplyDamageFromHit(const FName BoneName, float Damage);

	UFUNCTION(BlueprintPure)
	float GetBodyPartHealth(FGameplayTag BodyPartTag);
	FBodyPartHealthData* FindBodyPart(FGameplayTag BodyPartTag);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TArray<FBodyPartHealthData> DefaultBodyParts;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|Body Parts")
	TObjectPtr<UFretteBonesToTagData> BoneTagDataAsset;

	UPROPERTY(ReplicatedUsing = OnRep_BodyParts, BlueprintReadOnly)
	TArray<FBodyPartHealthData> BodyParts;

public:
	UFUNCTION()
	void OnRep_BodyParts();

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(FGameplayTag BodyPartTag, float Damage);

	FGameplayTag GetBodyPartFromBoneName(FName BoneName) const;
};