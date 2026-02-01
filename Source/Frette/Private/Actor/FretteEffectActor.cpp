


#include "Actor/FretteEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AFretteEffectActor::AFretteEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AFretteEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFretteEffectActor::ApplyEffectToTarget(AActor* TargetActor,
	TTuple<TSubclassOf<UGameplayEffect>, FEffectPolicyPair> EffectPair)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (TargetASC == nullptr) return;
	check(EffectPair.Key);
	
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(EffectPair.Key, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite; 	
	if (bIsInfinite && EffectPair.Value.RemovalPolicy != EEffectRemovalPolicy::DefaultRemove)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle,TargetASC);
	}
	
	if (bDestroyOnApplyEffect)
	{
		Destroy();
	}
}

void AFretteEffectActor::OnOverlap(AActor* TargetActor)
{
	for (const TTuple EffectPair : AppliedEffects)
	{
		if (EffectPair.Value.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor,EffectPair);
		}
	}
}

//Seul les effecta vec un duration infini vont être retirer de cette manière, les autres son gerer par unreal automatiquement
void AFretteEffectActor::RemoveGameplayEffectsFromTarget(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!IsValid(TargetASC)) return;
		
	TArray<FActiveGameplayEffectHandle> HandlesToRemove;
	for (TTuple HandlePair : ActiveEffectHandles)
	{
		if (TargetASC == HandlePair.Value)
		{
			TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
			HandlesToRemove.Add(HandlePair.Key);
		}
	}
		
	for (FActiveGameplayEffectHandle& HandleToRemove : HandlesToRemove)
	{
		ActiveEffectHandles.Remove(HandleToRemove);
	}
}

void AFretteEffectActor::OnEndOverlap(AActor* TargetActor)
{
	for (const TTuple EffectPair  : AppliedEffects)
	{
		if (EffectPair.Value.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor,EffectPair);
		}
		
		if (EffectPair.Value.RemovalPolicy != EEffectRemovalPolicy::DefaultRemove)
		{
			RemoveGameplayEffectsFromTarget(TargetActor);
		}
	}
}


