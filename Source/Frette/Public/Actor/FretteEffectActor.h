#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "FretteEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	DefaultRemove,
	RemoveOnEndOverlap,
};

USTRUCT(BlueprintType)
struct FEffectPolicyPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEffectApplicationPolicy ApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	//Seulement utile si l'éffet a une duration infini
	//Mais je veux pas vraiment faire plusieurs liste d'éffet selon leur type juste pour gerer ça séparément
	//Pourrais updater la visualisation dans l'éditeur selon le type de duration du gameplayEffect peut-être
	UPROPERTY(EditAnywhere)
	EEffectRemovalPolicy RemovalPolicy = EEffectRemovalPolicy::DefaultRemove;
};

//Pour faire des acteurs qui on un OnOverlap et qui applique des gameplay effects aux acteur qui entre en collision avec eux
UCLASS()
class FRETTE_API AFretteEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AFretteEffectActor();

protected:
	void ApplyEffectToTarget(AActor* TargetActor, const TTuple<TSubclassOf<UGameplayEffect>, FEffectPolicyPair>& EffectType);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	void RemoveGameplayEffectsFromTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frette|Applied Effects")
	bool bDestroyOnApplyEffect = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frette|Applied Effects")
	TMap<TSubclassOf<UGameplayEffect>, FEffectPolicyPair> AppliedEffects;

	TMap<FActiveGameplayEffectHandle, TObjectPtr<UAbilitySystemComponent>> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frette|Applied Effects")
	float ActorLevel = 1.f;
};