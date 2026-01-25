

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

UCLASS()
class FRETTE_API AFretteEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFretteEffectActor();

protected:
	virtual void BeginPlay() override;
	void ApplyEffectToTarget(AActor* TargetActor,TTuple<TSubclassOf<UGameplayEffect>, FEffectPolicyPair> GameplayEffectClass);
	
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	void RemoveGameplayEffectsFromTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnApplyEffect = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Applied Effects")
	TMap<TSubclassOf<UGameplayEffect>,FEffectPolicyPair> AppliedEffects;
	
	TMap<FActiveGameplayEffectHandle,TObjectPtr<UAbilitySystemComponent>> ActiveEffectHandles;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Applied Effects")
	float ActorLevel = 1.f;
};
