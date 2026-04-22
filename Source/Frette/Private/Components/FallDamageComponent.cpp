#include "Components/FallDamageComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/FretteBaseCharacter.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Components/BodyPart/FretteBodyPartTags.h"
#include "Frette/Frette.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Effect_Movement_FallDamage, "Effect.Movement.FallDamage");

void UFallDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	check(OwnerCharacter);

	MovementComponent = OwnerCharacter->GetCharacterMovement();
	check(MovementComponent);
	
	if (Config == nullptr)
	{
		FRETTE_LOG(Error, "Missing Fall Damage Config for %s.", OwnerCharacter->GetName());
	}

	BindMovementEvents();
}

void UFallDamageComponent::BindMovementEvents()
{
	OwnerCharacter->MovementModeChangedDelegate.AddDynamic(this, &UFallDamageComponent::MovementModeChanged);
	OwnerCharacter->LandedDelegate.AddDynamic(this, &UFallDamageComponent::Landed);
	OwnerCharacter->OnReachedJumpApex.AddDynamic(this, &UFallDamageComponent::OnJumpApexReached);
}

void UFallDamageComponent::MovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (MovementComponent->MovementMode == MOVE_Falling)
	{
		//Le notifyApex se fait set a false chaque foit que le notifyApex est triggered
		//donc il faut le re set a true au début des jumps
		MovementComponent->bNotifyApex = true;
		FallStartHeight = OwnerCharacter->GetActorLocation().Z;
	}
}

void UFallDamageComponent::Landed(const FHitResult& Hit)
{
	ApplyFallDamage(Hit.ImpactPoint.Z - FallStartHeight);
}

//Override le FallStartHeight a l'apex du saut 
void UFallDamageComponent::OnJumpApexReached()
{
	FallStartHeight = OwnerCharacter->GetActorLocation().Z;
}

int32 UFallDamageComponent::FindWorstDamageThresholdIndex(float DistanceFallen) const
{
	int32 WorstThresholdIndex = INDEX_NONE;
	for (int32 i = 0; i < Config->DamageThresholds.Num(); ++i)
	{
		if (DistanceFallen <= Config->DamageThresholds[i].FallHeight)
		{
			WorstThresholdIndex = i;
		}
		else
		{
			break;
		}
	}
	return WorstThresholdIndex;
}

//Lerp le dégat selon le threshold précédent
float UFallDamageComponent::CalculateFinalDamage(const float DistanceFallen, const int32 WorstThresholdIndex, const FFallDamageThreshold& Threshold) const
{
	float FinalDamage = Threshold.DamageValue;
	const int32 PrevIndex = WorstThresholdIndex - 1;
	if (PrevIndex >= 0)
	{
		const FFallDamageThreshold& PrevThreshold = Config->DamageThresholds[PrevIndex];
		const float Alpha = FMath::GetMappedRangeValueClamped(
			FVector2D(PrevThreshold.FallHeight, Threshold.FallHeight),
			FVector2D(0.f, 1.f),
			DistanceFallen
			);
		FinalDamage = FMath::Lerp(PrevThreshold.DamageValue, Threshold.DamageValue, Alpha);
	}
	return FinalDamage;
}

void UFallDamageComponent::ApplyFallDamage(float DistanceFallen) const
{
	if (DistanceFallen <= Config->DamageThresholds[0].FallHeight)
		return;

	const int32 WorstThresholdIndex = FindWorstDamageThresholdIndex(DistanceFallen);
	
	const FFallDamageThreshold& Threshold = Config->DamageThresholds[WorstThresholdIndex];
	
	float FallDamage = CalculateFinalDamage(DistanceFallen, WorstThresholdIndex, Threshold);

	const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(Threshold.Effect, 1, EffectContext);
	ensureMsgf(NewHandle.IsValid(), TEXT("Probably need to set the DamageEffect in the Config"));
	
	UFretteBodyPartComponent* BodyPartComponent = Cast<UFretteBodyPartComponent>(OwnerCharacter->GetComponentByClass(UFretteBodyPartComponent::StaticClass()));
	const float SeparatedDamage = FallDamage / Threshold.AffectedBones.Num();
	for (const FGameplayTag BoneTag : Threshold.AffectedBones)
	{
		BodyPartComponent->AddValueFromBodyPartTag(BoneTag, -SeparatedDamage, TAG_BodyPartValues_Health);
		FRETTE_LOG(Log, "%s took %d damage from falling %f cm", *OwnerCharacter->GetName(), FMath::TruncToInt32(SeparatedDamage), DistanceFallen);
	}
}