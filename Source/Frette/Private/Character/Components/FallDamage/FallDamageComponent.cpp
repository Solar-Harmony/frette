#include "Character/Components/FallDamage/FallDamageComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Frette.h"
#include "Character/FretteBaseCharacter.h"
#include "Character/Components/BodyParts/FretteBodyPartComponent.h"
#include "Character/Components/BodyParts/FretteBodyPartTags.h"

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
	
	if (Config->DamageThresholds.IsEmpty())
	{
		this->Deactivate();
		FRETTE_LOG(Warning, "No damage thresholds set for %s.).", OwnerCharacter->GetName());
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

//Va chercher le threshold de dégats le plus sévère que le joueur a atteint selon la distance tombé
int32 UFallDamageComponent::FindWorstDamageThresholdIndex(float DistanceFallen) const
{
	return Config->DamageThresholds.FindLastByPredicate([DistanceFallen](const FFallDamageThreshold& Threshold) {
			return DistanceFallen <= Threshold.FallHeight;
		});
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
	if (Config->DamageThresholds.Num() == 0 || DistanceFallen >= Config->DamageThresholds[0].FallHeight)
		return;

	const int32 WorstThresholdIndex = FindWorstDamageThresholdIndex(DistanceFallen);
	
	if (WorstThresholdIndex == INDEX_NONE)
		return;
	
	const FFallDamageThreshold& Threshold = Config->DamageThresholds[WorstThresholdIndex];

	const float FallDamage = CalculateFinalDamage(DistanceFallen, WorstThresholdIndex, Threshold);
	
	UFretteBodyPartComponent* BodyPartComponent = OwnerCharacter->GetComponentByClass<UFretteBodyPartComponent>();
	
	for (const FGameplayTag BoneTag : Threshold.AffectedBones)
	{
		BodyPartComponent->AddValueFromBodyPartTag(BoneTag, -FallDamage, TAG_BodyPartValues_Health);
		FRETTE_LOG(Log, "%s took %d damage from falling %f cm", *OwnerCharacter->GetName(), FMath::TruncToInt32(FallDamage), DistanceFallen);
	}
}