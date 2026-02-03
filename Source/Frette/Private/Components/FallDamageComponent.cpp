#include "Components/FallDamageComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/FretteBaseCharacter.h"

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

void UFallDamageComponent::ApplyFallDamage(float DistanceFell) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	if (DistanceFell > Config->MinFallHeight)
		return;

	ensureMsgf(Config, TEXT("FallDamageComponent missing Config Data Asset"));

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(Config->FallDamageEffect, 1, EffectContext);

	const float DamageAmount = Config->DamageCurve.GetRichCurveConst()->Eval(FMath::Abs(DistanceFell));

	NewHandle.Data->SetSetByCallerMagnitude(TAG_Effect_Movement_FallDamage, -DamageAmount);

	ensureMsgf(NewHandle.IsValid(), TEXT("Probably need to set the DamageEffect in the Config"));

	//TODO: Les dégats vont devoir être appliqué aux jambes plutot qu'au character en général
	ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC);
}