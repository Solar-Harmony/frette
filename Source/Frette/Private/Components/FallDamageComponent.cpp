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

void UFallDamageComponent::ApplyFallDamage(float DistanceFallen) const
{
	ensureMsgf(Config, TEXT("FallDamageComponent missing Config Data Asset"));
	if (DistanceFallen > Config->MinFallHeight)
		return;

	const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(Config->FallDamageEffect, 1, EffectContext);
	ensureMsgf(NewHandle.IsValid(), TEXT("Probably need to set the DamageEffect in the Config"));
	
	const float DamageAmount = Config->DamageCurve.GetRichCurveConst()->Eval(FMath::Abs(DistanceFallen));
	NewHandle.Data->SetSetByCallerMagnitude(TAG_Effect_Movement_FallDamage, -DamageAmount);
	
	UFretteBodyPartComponent* BodyPartComponent = Cast<UFretteBodyPartComponent>(OwnerCharacter->GetComponentByClass(UFretteBodyPartComponent::StaticClass()));
	for (const FGameplayTag BoneTag : Config->AffectedBones)
	{
		BodyPartComponent->AddValueFromBodyPartTag(BoneTag, -DamageAmount, TAG_BodyPartValues_Health);
	}
	
	UE_LOG(LogFrette, Verbose, TEXT("%s took %d damage from falling %f cm"), *OwnerCharacter->GetName(), FMath::TruncToInt32(DamageAmount), DistanceFallen);
}